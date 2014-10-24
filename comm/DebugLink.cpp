#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "util.h"
#include "DebugLink.h"

DebugLink::DebugLink() :
	logFile("debuglink_local.log", std::ios::out | std::ios::app),
	server(this, 11000)
{
	this->server.start();
}

DebugLink::~DebugLink()
{
	if (this->logFile.is_open())
	{
		this->logFile.close();
	}
}

DebugLink &DebugLink::instance()
{
	static DebugLink instance;
	return instance;
}

void DebugLink::close()
{
	this->server.stop();
}

void DebugLink::msg(int level, const std::string message)
{
	size_t len = (size_t) message.length();

	if (len > 65535)
	{
		len = 65535;
		this->msg(DebugLink::LEVEL_WARN, "DebugLink: Outgoing message exceeds 65535 bytes, ignoring the rest");
	}

	uint8_t data[len + 4];

	data[0] = DebugLink::PROTOCOL_TYPE_MSG;
	data[1] = level;
	data[2] = (len >> 8) & 0xFF;
	data[3] = len & 0xFF;

	for (int i = 0; i < len; ++i)
	{
		data[i + 4] = static_cast<uint8_t>(message[i]);
	}

	this->server.broadcast(data, len + 4);
	this->localMsg(level, message);
}

void DebugLink::event(int event)
{
	if (event > 65535)
	{
		// Cannot send event of this type
		return;
	}

	uint8_t data[] = {
		DebugLink::PROTOCOL_TYPE_EVENT,
		(uint8_t) ((event >> 8) & 0xFF),
		(uint8_t) ((event >> 0) & 0xFF)
	};

	this->server.broadcast(data, 3);
	this->localMsg(DebugLink::LEVEL_INFO, std::string("Emitting event ")
		.append(std::to_string(event)));
}

void DebugLink::object(int sequence, BaseObject *object)
{
}

void DebugLink::frame(Frame *frame)
{
	int width = 640;
	int height = 360;

	cv::Mat mat;
	cv::resize(*frame->sourceMat, mat, cv::Size(width, height));

	std::vector<unsigned char> buffer;
	cv::imencode(".jpg", mat, buffer);

	uint8_t header[] = {
		DebugLink::PROTOCOL_TYPE_FRAME,

		// Frame sequence number
		(uint8_t) ((frame->sequence >> 24) & 0xFF),
		(uint8_t) ((frame->sequence >> 16) & 0xFF),
		(uint8_t) ((frame->sequence >> 8) & 0xFF),
		(uint8_t) (frame->sequence & 0xFF),

		// Frame height
		(uint8_t) ((frame->hsvMat->size().height >> 8) & 0xFF),
		(uint8_t) (frame->hsvMat->size().height & 0xFF),

		// Image width
		(uint8_t) ((width >> 8) & 0xFF),
		(uint8_t) (width & 0xFF),

		// Image height
		(uint8_t) ((height >> 8) & 0xFF),
		(uint8_t) (height & 0xFF),

		(uint8_t) ((buffer.size() >> 24) & 0xFF),
		(uint8_t) ((buffer.size() >> 16) & 0xFF),
		(uint8_t) ((buffer.size() >> 8) & 0xFF),
		(uint8_t) (buffer.size() & 0xFF)
	};

	this->server.broadcast(header, 15);
	this->server.broadcast(&buffer[0], buffer.size());

	for (std::vector<BaseObject *>::size_type i = 0; i < frame->objects.size(); ++i)
	{
		object(frame->sequence, frame->objects[i]);
	}
}

void DebugLink::localMsg(int level, const std::string message)
{
	std::string levelStr;

	switch (level)
	{
		case DebugLink::LEVEL_INFO: levelStr = "INFO"; break;
		case DebugLink::LEVEL_WARN: levelStr = "WARN"; break;
		case DebugLink::LEVEL_ERROR: levelStr = "ERROR"; break;
		default: levelStr = "DEBUG";
	}

	char log[2048];
	snprintf(log, 2048, "[%llu] %s: %s", microtime() / 1000, levelStr.c_str(), message.c_str());

	if (this->logFile.is_open())
	{
		this->logFile << log << "\n";
	}

	std::cout << log << std::endl;
}

DebugLink::Server::Server(DebugLink *dl, int port) :
	debug(dl),
	port(port),
	isRunning(false)
{
}

DebugLink::Server::~Server()
{
	this->stop();
}

void DebugLink::Server::start()
{
	if (this->isRunning)
	{
		return;
	}

	this->isRunning = true;
	this->thread = new std::thread(&DebugLink::Server::listen, this);

	this->debug->msg(DebugLink::LEVEL_INFO, "DebugLink::Server: Started server thread");
}

void DebugLink::Server::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread->join();
	this->clients.clear();

	this->debug->msg(DebugLink::LEVEL_INFO, "DebugLink::Server: Stopped server thread");

	delete this->thread;
}

int DebugLink::Server::broadcast(const uint8_t *buf, size_t size)
{
	int success = 0;
	std::lock_guard<std::mutex> lock(this->clientsMutex);

	for (std::vector<int>::size_type i = 0; i < this->clients.size();)
	{
		int fd = this->clients[i];

		if (write(fd, buf, size) != (ssize_t) size)
		{
			::close(fd);
			this->clients.erase(this->clients.begin() + i);
		}
		else
		{
			++i;
			++success;
		}
	}

	return success;
}

void DebugLink::Server::listen()
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
	this->debug->msg(DebugLink::LEVEL_INFO, "DebugLink::Server: Listening for incoming connections");

	fd_set fds;
	timeval tv;

	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	while (this->isRunning && select(0, &fds, NULL, NULL, &tv) >= 0)
	{
		size_t size = sizeof(struct sockaddr_in);
		struct sockaddr_in client_addr;
		int clientfd = accept(sockfd, NULL, NULL);

		if (clientfd == -1)
		{
			continue;
		}

		this->debug->msg(DebugLink::LEVEL_INFO, "DebugLink::Server: Incoming connection");

		std::lock_guard<std::mutex> lock(this->clientsMutex);
		this->clients.push_back(clientfd);
	}

	::close(sockfd);
	this->debug->msg(DebugLink::LEVEL_INFO, "DebugLink::Server: Server socket closed");
}
