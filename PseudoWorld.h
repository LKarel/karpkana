#ifndef PSEUDOWORLD_H
#define PSEUDOWORLD_H

#define PSEUDOWORLD_MIN_AGE 2

#include <cmath>
#include <map>
#include "main.h"
#include "util.h"
#include "vp/VideoFrame.h"

class PseudoWorld
{
public:
	class Ball
	{
	public:
		Ball() : sequence(0), age(0), pos{0}
		{
		}

		bool inTrackingRegion(PseudoWorld::Ball &ball) const;
		bool inTribblerRegion() const;

		int sequence;
		int age;
		int radius;

		RelPosition pos;
		Point2d blob;
	};

	class Goal
	{
	public:
		Goal() : sequence(0), halfwidth(0), visible(false), pos{0}
		{
		}

		int sequence;
		int halfwidth;
		bool visible;

		RelPosition pos;
	};

	PseudoWorld();
	~PseudoWorld();

	void onFrame(VideoFrame *frame);
	bool hasBalls() const;
	PseudoWorld::Ball *getBall(int id) const;
	int getAge() const;

	int targetColor;
	std::map<int, PseudoWorld::Ball *> balls;
	PseudoWorld::Goal target;

private:
	int ids;
	int age;

	void readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob);
	void readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob);
};

#endif
