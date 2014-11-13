#ifndef PID_H
#define PID_H

class PID
{
public:
	PID(double p, double i, double d);
	~PID();

	void reset();
	double update(double error);

private:
	double p;
	double i;
	double d;

	double prevError;
	double integral;
};

#endif
