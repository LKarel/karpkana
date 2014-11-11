#ifndef WORLD_H
#define WORLD_H

#include <cmath>
#include <vector>
#include "main.h"
#include "util.h"
#include "vp/VideoFrame.h"

class World
{
public:
	class Ball
	{
	public:
		Ball() : id(0), sequence(0), age(0), pos{0}
		{
		}

		bool inTrackingBox(Ball *ball);

		int id;
		int sequence;
		int age;

		RelPosition pos;
	};

	class Goal
	{
	public:
		Goal() : visible(false), sequence(0), pos{0}
		{
		}

		bool visible;
		int sequence;

		RelPosition pos;
	};

	World();
	~World();

	void onFrame(VideoFrame *frame);

	int targetColor;
	std::vector<World::Ball *> balls;
	World::Goal target;

private:
	int ids;

	RelPosition pointToWorldPos(const Point2d &point) const;

	void readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob);
	void readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob);
};

#endif
