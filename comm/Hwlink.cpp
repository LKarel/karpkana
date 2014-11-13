#include "comm/Hwlink.h"

Hwlink::Hwlink(const char *device) :
	id(0),
	fd(-1)
{
#if HW_SIMULATE != 1
	this->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);

	struct termios tty;

	cfsetospeed(&tty, (speed_t) B115200);
	cfsetispeed(&tty, (speed_t) B115200);

	tty.c_cflag &= ~PARENB; // Disable parity checks
	tty.c_cflag &= ~CSTOPB; // One stop bit
	tty.c_cflag &= ~CSIZE; // Character size mask
	tty.c_cflag |= CS8; // 8 data bits

	tty.c_cflag &= ~CRTSCTS; // Disable flow control
	tty.c_cc[VMIN] = 1; // Non-blocking read
	tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

	cfmakeraw(&tty);
	tcflush(fd, TCIFLUSH);

	if (tcsetattr(this->fd, TCSANOW, &tty) == -1)
	{
		Log::perror("Hwlink: Setting TTY attributes for device %s", device);

		close(this->fd);
		this->fd = -1;

		return;
	}

	this->command("?");

	long begin = microtime();

	while (!this->id && microtime() - begin < COMM_HWLINK_TIMEOUT)
	{
		this->tick();

		while (!this->id && !this->messages.empty())
		{
			char *msg = this->messages.front();

			if (sscanf(msg, "<id:%d>", &this->id) == 1)
			{
				Log::printf("Hwlink: found device with ID %d", this->id);
			}

			free(msg);
			this->messages.pop();
		}

		usleep(10000);
	}

	if (!this->id)
	{
		Log::printf("Hwlink: error: timeout while waiting for ID");

		close(this->fd);
		this->fd = -1;
	}
#endif
}

#if HW_SIMULATE == 1
Hwlink::Hwlink(int id) : id(id), fd(-1)
{
}
#endif

Hwlink::~Hwlink()
{
#if HW_SIMULATE != 1
	if (this->fd > 0)
	{
		close(this->fd);
	}
#endif

	while (!this->messages.empty())
	{
		delete this->messages.front();
		this->messages.pop();
	}
}

bool Hwlink::isOpen()
{
#if HW_SIMULATE == 1
	return true;
#else
	return this->fd > 0;
#endif
}

void Hwlink::tick()
{
#if HW_SIMULATE != 1
	char buf[32];
	ssize_t count;

	count = read(this->fd, &buf, 32);

	if (count == -1)
	{
		if (errno == EAGAIN)
		{
			return;
		}

		Log::perror("Hwlink: read error from %d", fd);
		return;
	}

	for (ssize_t i = 0; i < count; i++)
	{
		if (buf[i] == '\n' || buf[i] == '\r')
		{
			if (this->buffer.empty())
			{
				continue;
			}

			this->buffer.push_back(0);

			char *message = (char *) malloc(strlen(&this->buffer[0]) + 1);
			strcpy(message, &this->buffer[0]);

			this->messages.push(message);
			this->buffer.clear();
		}
		else
		{
			this->buffer.push_back(buf[i]);
		}
	}
#endif
}

void Hwlink::command(const char *fmt, ...)
{
	if (!this->isOpen())
	{
		Log::printf("Hwlink: error: link not open");
		return;
	}

	size_t len = strlen(fmt) + 2;
	char myfmt[len];
	char cmd[64];

	snprintf(myfmt, len, "%s\n", fmt);

	va_list args;
	va_start(args, fmt);
	vsnprintf(cmd, 64, myfmt, args);
	va_end(args);

	char debug[64];
	snprintf(debug, strlen(cmd), "%s", cmd);

	Log::printf("Hwlink: transmitting: %s", debug);

#if HW_SIMULATE != 1
	if (write(this->fd, cmd, strlen(cmd)) == -1)
	{
		Log::perror("Hwlink: sending command");
		return;
	}
#endif
}
