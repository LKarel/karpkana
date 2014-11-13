#include "PID.h"

PID::PID(double p, double i, double d) :
	p(p),
	i(i),
	d(d),
	prevError(0),
	integral(0)
{
}

PID::~PID()
{
}

void PID::reset()
{
	this->prevError = 0;
	this->integral = 0;
}

double PID::update(double error)
{
	double pid = error * this->p + this->integral * this->i + (error - this->prevError) * this->d;

	this->prevError = error;
	this->integral += error;

	return pid;
}
