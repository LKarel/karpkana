#ifndef WORLD_H
#define WORLD_H

#include <cmath>
#include <vector>
#include "vp/VideoFrame.h"

class World
{
public:
	class Ball
	{
	public:
		Ball() : id(0), sequence(0), distance(0), angle(0), blobx(0), bloby(0)
		{
		}

		bool inTrackingBox(Ball *ball);

		int id;
		int sequence;
		int distance;
		double angle;

		int blobx;
		int bloby;
	};

	World();
	~World();

	void onFrame(VideoFrame *frame);

	std::vector<World::Ball *> balls;

private:
	int ids;
};

#endif
