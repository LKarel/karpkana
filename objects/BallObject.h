#ifndef OBJECTS_BALLOBJECT_H
#define OBJECTS_BALLOBJECT_H

#include "objects/BaseObject.h"

class BallObject : BaseObject
{
public:
	static const int TYPE = 0x2;

	static BallObject *createByVisual(int x, int y, int radius);

	BallObject();
	~BallObject();

	class Visual
	{
	public:
		int x;
		int y;
		int radius;
	};

	class Real
	{
	public:
		int distance;
		int offset;
		double angle;

	private:
		void calculate(BallObject::Visual visual);
	};

	Visual visual;
	Real real;
};

#endif
