#include "Robot.h"

Robot::Robot(const char *type) :
	motors{NULL},
	coilgun(NULL)
{
#if HW_SIMULATE == 1
	Log::printf("Robot: in hardware simulation mode, no actual devices connected");

	this->motors[MOTOR_A] = new Motor(new Hwlink(MOTOR_A));
	this->motors[MOTOR_B] = new Motor(new Hwlink(MOTOR_B));
	this->motors[MOTOR_C] = new Motor(new Hwlink(MOTOR_C));
	this->coilgun = new Coilgun(new Hwlink(COILGUN));
#else
	int motors_count = 0;
	struct dirent **names;
	ssize_t count = scandir("/dev", &names, NULL, alphasort);

	if (count == -1)
	{
		Log::perror("Robot: could not list /dev");
		return;
	}

	for (ssize_t i = 0; i < count; i++)
	{
		if (strstr(names[i]->d_name, type) == names[i]->d_name)
		{
			Log::printf("Robot: found %s", names[i]->d_name);

			char fullpath[64];
			snprintf(fullpath, 64, "/dev/%s", names[i]->d_name);

			Hwlink *link = new Hwlink(fullpath);

			if (!link->id)
			{
				Log::printf("Robot: failed to identify device");
				delete link;
				continue;
			}

			Log::printf("Robot: loaded %s with ID %d", names[i]->d_name, link->id);

			switch (link->id)
			{
				case MOTOR_A:
				case MOTOR_B:
				case MOTOR_C:
				{
					this->motors[link->id] = new Motor(link);
					motors_count++;
				}
				break;

				case COILGUN:
				{
					this->coilgun = new Coilgun(link);
				}
				break;

				default:
				{
					Log::printf("Robot: unknown ID, unloading device");
					delete link;
				}
				break;
			}
		}
	}

	if (motors_count != 3)
	{
		Log::printf("Robot: warning: not all motors loaded");
	}

	if (!this->coilgun)
	{
		Log::printf("Robot: warning: coilgun not loaded");
	}
#endif
}

Robot::~Robot()
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


void Robot::rotate(int speed)
{
	for (size_t i = 0; i < MOTORS_NUM; i++)
	{
		if (this->motors[i])
		{
			if (speed == 0)
			{
				this->motors[i]->stop();
			}
			else
			{
				this->motors[i]->setSpeed(speed);
			}
		}
	}
}

void Robot::stop()
{
	this->rotate(0);
}

void Robot::direction(int direction, int speed)
{
	if (direction == DIRECTION_FWD)
	{
		this->motors[MOTOR_A]->setSpeed(speed);
		this->motors[MOTOR_B]->setSpeed(-speed);
		this->motors[MOTOR_C]->setSpeed(0);
	}
}

void Robot::rotateForward(int fwd, int rotate)
{
	this->motors[MOTOR_A]->setSpeed(fwd - rotate);
	this->motors[MOTOR_B]->setSpeed(-fwd + rotate);
	this->motors[MOTOR_C]->setSpeed(rotate);
}
