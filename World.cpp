#include "World.h"

#define TRACKING_RADIUS 65

//#define CAM_VFOV 0.598647933
//#define CAM_HFOV 1.06465084
//#define CAM_ANGLE 0.586154177
//#define CAM_HEIGHT 194.48

#define CAM_VFOV 0.32070425
#define CAM_HFOV 0.570286333
#define CAM_ANGLE 0.53804420679
#define CAM_HEIGHT 305

#define CAPT_CENTER_Y (CAPT_HEIGHT / 2.0)
#define CAPT_CENTER_X (CAPT_WIDTH / 2.0)

World::World() :
	targetColor(VideoFrame::Blob::COLOR_YELLOW),
	ids(1)
{
}

World::~World()
{
	for (std::vector<World::Ball *>::size_type i = 0; i < this->balls.size(); i++)
	{
		delete this->balls[i];
	}
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
			delete this->balls[i];
			this->balls.erase(this->balls.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

RelPosition World::pointToWorldPos(const Point2d &point) const
{
	double distance = CAM_HEIGHT / tan((CAM_VFOV * (point.y - CAPT_CENTER_Y) / CAPT_CENTER_Y) + CAM_ANGLE);
	double hordev = ((tan(CAM_HFOV) * distance) * (point.x - CAPT_CENTER_X) / CAPT_CENTER_X);

	return {sqrt(pow(distance, 2) + pow(hordev, 2)), atan(hordev / distance)};
}

void World::readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	World::Ball *ball = new World::Ball();

	ball->sequence = frame->sequence;
	ball->age = 0;

	// Calculate real-world position
	Point2d ballPoint = {(blob->x1 + blob->x2) / 2.0, (blob->y1 + blob->y2) / 2.0};
	ball->pos = this->pointToWorldPos(ballPoint);

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

	//printf("id=%d\tradius=%f\tangle=%f\n", ball->id, ball->pos.radius, ball->pos.angle);
}

void World::readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	if (blob->color != this->targetColor)
	{
		return;
	}

	this->target.visible = true;
	this->target.sequence = frame->sequence;

	Point2d center = {
		(blob->y1 < blob->y2) ? (double) blob->y1 : (double) blob->y2,
		(blob->x1 + blob->x2) / 2.0
	};

	this->target.pos = this->pointToWorldPos(center);

	//printf("goal radius=%f\tangle=%f\n", this->target.pos.radius, this->target.pos.angle);
}

bool World::Ball::inTrackingBox(Ball *ball)
{
	return relPositionDistance(ball->pos, this->pos) <= TRACKING_RADIUS;
}
