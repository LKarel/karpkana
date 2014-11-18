#include "Kbui.h"

Kbui::Kbui(int fd) : isRunning(true), fd(fd)
{
	int flags;
	flags = fcntl(this->fd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(this->fd, F_SETFL, flags);

	this->thread = std::thread(&Kbui::run, this);
}

Kbui::~Kbui()
{
	close(this->fd);
}

void Kbui::stop()
{
	if (this->isRunning)
	{
		this->isRunning = false;
		this->thread.join();
	}
}

int Kbui::cmd()
{
	std::lock_guard<std::mutex> lock(this->cmdsMutex);

	int cmd = 0;

	if (!this->cmds.empty())
	{
		cmd = this->cmds.front();
		this->cmds.pop();
	}

	return cmd;
}

void Kbui::run()
{
	while (this->isRunning)
	{
		char ch = 0;

		if (read(this->fd, &ch, 1) != 1)
		{
			usleep(1000);
			continue;
		}

		if (ch > 0)
		{
			std::lock_guard<std::mutex> lock(this->cmdsMutex);

			switch (ch)
			{
				case 's': this->cmds.push(Kbui::CMD_STOP); break;
				case 'b': this->cmds.push(Kbui::CMD_BEGIN); break;
			}
		}
	}
}
