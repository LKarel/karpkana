#include "Kbui.h"

Kbui::Kbui() : isRunning(true), cmd(0)
{
	this->thread = std::thread(&Kbui::run, this);
}

Kbui::~Kbui()
{
}

void Kbui::stop()
{
	if (this->isRunning)
	{
		this->isRunning = false;
		this->thread.join();
	}
}

int Kbui::getCommand()
{
	std::lock_guard<std::mutex> lock(this->cmdMutex);

	int cmd = this->cmd;
	this->cmd = 0;

	return cmd;
}

void Kbui::run()
{
	char ch;

	while (this->isRunning)
	{
		ch = fgetc(stdin);

		// Wait until the previous command has been handled
		while (this->isRunning)
		{
			usleep(10000);
			std::lock_guard<std::mutex> lock(this->cmdMutex);

			if (!this->cmd)
			{
				switch (ch)
				{
					case 's': this->cmd = Kbui::CMD_STOP; break;
					case 'b': this->cmd = Kbui::CMD_BEGIN; break;
				}

				break;
			}
		}
	}
}
