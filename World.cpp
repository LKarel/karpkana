#include "World.h"

#define TRACKING_RADIUS 65

//#define CAM_VFOV 0.598647933
//#define CAM_HFOV 1.06465084
//#define CAM_ANGLE 0.586154177
//#define CAM_HEIGHT 194.48

#define CAM_VFOV 0.300393263
#define CAM_HFOV 1.06465084
#define CAM_ANGLE 0.53804420679
#define CAM_HEIGHT 305

World::World() :
	targetColor(VideoFrame::Blob::COLOR_YELLOW),
	ids(1)
{
}

World::~World()
{
}

void World::onFrame(VideoFrame *frame)
{
	for (std::vector<VideoFrame::Blob *>::size_type i = 0; i < frame->blobs.size(); i++)
	{
		VideoFrame::Blob *blob = frame->blobs[i];

		switch (blob->color)
		{
			case VideoFrame::Blob::COLOR_BALL:
				this->readBallBlob(frame, blob);
				break;

			case VideoFrame::Blob::COLOR_YELLOW:
			case VideoFrame::Blob::COLOR_BLUE:
				this->readGoalBlob(frame, blob);
				break;
		}
	}

	std::vector<World::Ball *>::size_type i = 0;
	while (i < this->balls.size())
	{
		if (frame->sequence - this->balls[i]->sequence > 10)
		{
			// This ball has not been visible for some time
			this->balls.erase(this->balls.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

void World::readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	World::Ball *ball = new World::Ball();

	ball->sequence = frame->sequence;
	ball->age = 0;

	double y = (double) (blob->y1 + blob->y2) / 2;
	double x = (double) (blob->x1 + blob->x2) / 2;

	ball->distance = CAM_HEIGHT / tan((CAM_VFOV * (y / CAPT_HEIGHT)) + CAM_ANGLE);
	ball->angle = (CAM_HFOV * (x / (CAPT_WIDTH / 2))) - 1;

	ball->realx = ball->distance * sin(ball->angle);
	ball->realy = ball->distance * cos(ball->angle);

	// Verify if in any tracking area
	for (std::vector<World::Ball *>::size_type j = 0; j < this->balls.size(); j++)
	{
		if (frame->sequence != this->balls[j]->sequence &&
			this->balls[j]->inTrackingBox(ball))
		{
			ball->id = this->balls[j]->id;
			ball->age++;

			// Replace the previous ball
			delete this->balls[j];
			this->balls[j] = ball;

			break;
		}
	}

	if (!ball->id)
	{
		// This is a new ball
		ball->id = this->ids++;
		this->balls.push_back(ball);
	}

	if (frame->sequence - this->target.sequence > 5)
	{
		this->target.visible = false;
	}

	//printf("id=%d\tdistance: %d\tangle=%f\n", ball->id, ball->distance, ball->angle);
	//printf("id=%d\tx=%d\ty=%d\n", ball->id, ball->realx, ball->realy);
}

void World::readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	if (blob->color != this->targetColor)
	{
		return;
	}

	this->target.visible = true;
	this->target.sequence = frame->sequence;

	double y = (blob->y1 < blob->y2) ? blob->y1 : blob->y2;
	double x = (double) (blob->x1 + blob->x2) / 2;

	this->target.distance = CAM_HEIGHT / tan((CAM_VFOV * (y / CAPT_HEIGHT)) + CAM_ANGLE);
	this->target.angle = (CAM_HFOV * (x / (CAPT_WIDTH / 2))) - 1;

	this->target.realx = this->target.distance * sin(this->target.angle);
	this->target.realy = this->target.distance * cos(this->target.angle);

	//printf("goal distance=%d\tangle=%f\n", this->target.distance, this->target.angle);
}

bool World::Ball::inTrackingBox(Ball *ball)
{
	int distance = sqrt(pow(ball->realx - this->realx, 2) + pow(ball->realy - this->realy, 2));

	return distance <= TRACKING_RADIUS;
}
