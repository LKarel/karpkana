#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "util.h"
#include "comm/DebugLink.h"
#include "comm/DebugServer.h"

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
	return this->clients.size() > 0;
}

int DebugServer::broadcast(const uint8_t *buf, size_t size)
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
	DebugLink::instance().msg(DebugLink::LEVEL_INFO, "DebugServer: Listening for incoming connections");

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

		DebugLink::instance().msg(DebugLink::LEVEL_INFO, "DebugServer: Incoming connection");

		std::lock_guard<std::mutex> lock(this->clientsMutex);
		this->clients.push_back(clientfd);
	}

	::close(sockfd);
	DebugLink::instance().msg(DebugLink::LEVEL_INFO, "DebugServer: DebugServer socket closed");
}
