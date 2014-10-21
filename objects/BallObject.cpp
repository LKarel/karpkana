#include "BallObject.h"

BallObject::BallObject()
{
	this->type = BallObject::TYPE;
}

BallObject::~BallObject()
{
}

BallObject *BallObject::createByVisual(int x, int y, int radius)
{
	BallObject *ball = new BallObject();

	ball->visual.x = x;
	ball->visual.y = y;
	ball->visual.radius = radius;

	return ball;
}

void BallObject::Real::calculate(BallObject::Visual visual)
{
	// Need access to real hardware and test environment
}
