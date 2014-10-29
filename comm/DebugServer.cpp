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
		return;
	}

	// Enable the socket to reuse the address
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		return;
	}

	ioctl(sockfd, FIONBIO, &yes);

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
	{
		return;
	}

	if (::listen(sockfd, 5) == -1)
	{
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
					// TODO: Do something useful
					Log::printf("DebugServer: receiving data from client");
				}
				else
				{
					close(i);
					FD_CLR(i, &fds);

					Log::printf("DebugServer: client disconnected");
				}
			}
		}
	}

	::close(sockfd);
	Log::printf("DebugServer: server socket closed");
}
