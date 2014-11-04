#include "TestController.h"

TestController::TestController(VideoProcessor *vp) :
	vp(vp),
	isRunning(true)
{
	this->thread = std::thread(&TestController::run, this);
}

TestController::~TestController()
{
}

void TestController::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread.join();
}

void TestController::run()
{
	//HwScanner hw("ttyACM");

	//hw.motors[MOTOR_A]->setSpeed(Motor::SPEED_MAX * 0.5);
	//hw.motors[MOTOR_B]->setSpeed(Motor::SPEED_MAX * 0.5);
	//hw.motors[MOTOR_C]->setSpeed(Motor::SPEED_MAX * 0.5);

	//usleep(400000);
	//Log::printf("TestController: done");

	//return; // The destructors should auto-stop all motors

	while (this->isRunning)
	{
		VideoFrame *frame = this->vp->getFrame();

		if (!frame)
		{
			usleep(5000);
			continue;
		}

		this->world.onFrame(frame);

		delete frame;
	}
}
