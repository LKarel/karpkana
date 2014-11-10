#ifndef WORLD_H
#define WORLD_H

#include <cmath>
#include <vector>
#include "main.h"
#include "vp/VideoFrame.h"

class World
{
public:
	class Ball
	{
	public:
		Ball() : id(0), sequence(0), distance(0), angle(0), realx(0), realy(0)
		{
		}

		bool inTrackingBox(Ball *ball);

		int id;
		int sequence;
		int distance;
		double angle;
		int age;

		// Ball coordinates in real world
		int realx;
		int realy;
	};

	class Goal
	{
	public:
		Goal() : visible(false), sequence(0), distance(0), angle(0), realx(0), realy(0)
		{
		}

		bool visible;
		int sequence;
		int distance;
		double angle;
		int realx;
		int realy;
	};

	World();
	~World();

	void onFrame(VideoFrame *frame);

	int targetColor;
	std::vector<World::Ball *> balls;
	World::Goal target;

private:
	int ids;

	void readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob);
	void readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob);
};

#endif
