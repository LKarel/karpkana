#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "util.h"
#include "comm/DebugServer.h"

bool _isInvalidFd(int fd)
{
	return fd <= 0;
}

DebugServer::DebugServer() : isRunning(false)
{
}

DebugServer::~DebugServer()
{
	this->stop();
}

void DebugServer::start(int port)
{
	if (this->isRunning)
	{
		return;
	}

	this->port = port;
	this->isRunning = true;
	this->thread = new std::thread(&DebugServer::run, this);
}

void DebugServer::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread->join();
	this->clients.clear();

	delete this->thread;
}

bool DebugServer::hasClients()
{
	std::lock_guard<std::mutex> lock(this->clientsMutex);
	return this->clients.size() > 0;
}

void DebugServer::broadcast(const uint8_t *buf, size_t size)
{
	std::lock_guard<std::mutex> lock(this->clientsMutex);

	for (std::vector<int>::size_type i = 0; i < this->clients.size(); ++i)
	{
		if (write(this->clients[i], buf, size) != (ssize_t) size)
		{
			this->clients[i] = -1;
		}
	}

	// Remove negative values
	std::vector<int>::iterator it = std::remove_if(this->clients.begin(), this->clients.end(), _isInvalidFd);
    this->clients.resize(it - this->clients.begin());
}

google::protobuf::Message *DebugServer::getIncoming()
{
	std::lock_guard<std::mutex> lock(this->incomingMutex);
	google::protobuf::Message *msg = NULL;

	if (!this->incoming.empty())
	{
		msg = this->incoming.front();
		this->incoming.pop();
	}

	return msg;
}

void DebugServer::run()
{
	int yes = 1;
	int sockfd;
	struct addrinfo hints, *res;

	// Get address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, std::to_string(this->port).c_str(), &hints, &res) != 0)
	{
		return;
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1)
	{
		Log::perror("DebugServer: socket()");
		return;
	}

	// Enable the socket to reuse the address
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		Log::perror("DebugServer: set SO_REUSEADDR");
		return;
	}

	ioctl(sockfd, FIONBIO, &yes);

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
	{
		Log::perror("DebugServer: bind()");
		return;
	}

	if (::listen(sockfd, 5) == -1)
	{
		Log::perror("DebugServer: listen()");
		return;
	}

	freeaddrinfo(res);
	Log::printf("DebugServer: listening for incoming connections");

	fd_set fds;
	fd_set rfds;
	int fdmax = sockfd;
	uint8_t buf[1024];

	FD_ZERO(&fds);
	FD_ZERO(&rfds);
	FD_SET(sockfd, &fds);

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	while (this->isRunning)
	{
		rfds = fds; // Copy the master FD set

		if (select(fdmax + 1, &rfds, NULL, NULL, &tv) == -1)
		{
			Log::perror("select()");
			break;
		}

		for (int i = 0; i <= fdmax; ++i)
		{
			if (!FD_ISSET(i, &rfds))
			{
				// No events on descriptor i
				continue;
			}

			// New incoming connection
			if (i == sockfd)
			{
				int client = accept(sockfd, NULL, NULL);

				if (client == -1)
				{
					continue;
				}

				Log::printf("DebugServer: incoming connection");

				FD_SET(client, &fds);
				fdmax = (client > fdmax) ? client : fdmax;

				std::lock_guard<std::mutex> lock(this->clientsMutex);
				this->clients.push_back(client);
			}
			else
			{
				ssize_t count = recv(i, &buf, 1024, 0);

				if (count > 0)
				{
					DebugServer::Parser *parser = this->getParser(i);

					parser->handle((uint8_t *) buf, (size_t) count);

					while (!parser->messages.empty())
					{
						this->incoming.push(parser->messages.front());
						parser->messages.pop();
					}
				}
				else
				{
					close(i);
					FD_CLR(i, &fds);

					// TODO: Remove the client's parser

					Log::printf("DebugServer: client disconnected");
				}
			}
		}
	}

	::close(sockfd);
	Log::printf("DebugServer: server socket closed");
}

DebugServer::Parser *DebugServer::getParser(int client)
{
	std::lock_guard<std::mutex> lock(this->parsersMutex);
	std::map<int, Parser *>::iterator it = this->parsers.find(client);

	DebugServer::Parser *parser;

	if (it == this->parsers.end())
	{
		parser = new DebugServer::Parser();
		this->parsers[client] = parser;
	}
	else
	{
		parser = it->second;
	}

	return parser;
}

DebugServer::Parser::~Parser()
{
	while (!this->messages.empty())
	{
		delete this->messages.front();
		this->messages.pop();
	}
}

void DebugServer::Parser::handle(uint8_t *data, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (!this->type)
		{
			this->type = data[i];
			continue;
		}
		else if (this->lenReceived != 4)
		{
			this->len = (this->len << 8) | data[i];
			this->lenReceived++;

			if (this->lenReceived == 4 && !this->len)
			{
				// Empty message
				this->messages.push(this->parseProtobuf(this->type, NULL, 0));

				this->type = 0;
				this->lenReceived = 0;
			}

			continue;
		}

		if (!this->payload)
		{
			this->payload = (uint8_t *) malloc(this->len);
		}

		this->payload[this->payloadReceived++] = data[i];

		if (this->payloadReceived == this->len)
		{
			// End of a message
			this->messages.push(this->parseProtobuf(this->type, this->payload, this->len));

			// Clean up the parser state for the next message
			this->type = 0;
			this->len = 0;
			this->lenReceived = 0;
			this->payload = NULL;
			this->payloadReceived = 0;
		}
	}
}

google::protobuf::Message *DebugServer::Parser::parseProtobuf(int type, uint8_t *data, size_t len)
{
	google::protobuf::Message *msg = NULL;

	switch (type)
	{
		case PROTOCOL__REQUEST_COLORS:
			msg = new c22dlink::RequestColors();
			break;

		case PROTOCOL__COLOR_INFO:
			msg = new c22dlink::ColorInfo();
			break;

		case PROTOCOL__LOAD_COLORS:
			msg = new c22dlink::LoadColors();
			break;

		case PROTOCOL__SAVE_COLORS:
			msg = new c22dlink::SaveColors();
			break;
	}

	if (msg && data && len)
	{
		msg->ParseFromArray(data, len);
	}

	return msg;
}
