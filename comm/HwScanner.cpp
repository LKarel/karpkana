#include "comm/HwScanner.h"

HwScanner::HwScanner(const char *type) :
	motors{NULL},
	coilgun(NULL)
{
	int motors_count = 0;
	struct dirent **names;
	ssize_t count = scandir("/dev", &names, NULL, alphasort);

	if (count == -1)
	{
		Log::perror("HwScanner: could not list /dev");
		return;
	}

	for (ssize_t i = 0; i < count; i++)
	{
		if (strstr(names[i]->d_name, type) == names[i]->d_name)
		{
			Log::printf("HwScanner: found %s", names[i]->d_name);

			char fullpath[64];
			snprintf(fullpath, 64, "%s/%s", type, names[i]->d_name);

			Hwlink *link = new Hwlink(fullpath);

			if (!link->id)
			{
				Log::printf("HwScanner: failed to identify device");
				delete link;
				continue;
			}

			Log::printf("HwScanner: loaded %s with ID %d", names[i]->d_name, link->id);

			switch (link->id)
			{
				case MOTOR_A:
				case MOTOR_B:
				case MOTOR_C:
				{
					this->motors[motors_count++] = new Motor(link);
				}
				break;

				case COILGUN:
				{
					this->coilgun = new Coilgun(link);
				}
				break;

				default:
				{
					Log::printf("HwScanner: unknown ID, unloading device");
					delete link;
				}
				break;
			}
		}
	}

	if (motors_count != MOTORS_NUM)
	{
		Log::printf("HwScanner: warning: not all motors loaded");
	}
}

HwScanner::~HwScanner()
{
	for (size_t i = 0; i < MOTORS_NUM; i++)
	{
		if (this->motors[i])
		{
			delete this->motors[i];
		}
	}

	if (this->coilgun)
	{
		delete this->coilgun;
	}
}
