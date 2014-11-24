#include "PseudoWorld.h"

#define CAUGHT_TIMEOUT 300000
#define Y_SHIFT (CAPT_HEIGHT)

PseudoWorld::PseudoWorld() :
	targetColor(VideoFrame::Blob::COLOR_YELLOW),
	ids(1),
	age(0),
	lastFrame(0)
{
}

PseudoWorld::~PseudoWorld()
{
	std::map<int, PseudoWorld::Ball *>::iterator it = this->balls.begin();
	while (it != this->balls.end())
	{
		delete it->second;
		++it;
	}
}

void PseudoWorld::onFrame(VideoFrame *frame)
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

	std::map<int, PseudoWorld::Ball *>::iterator it = this->balls.begin();
	PseudoWorld::Ball *ball;

	// Removing balls that are no longer visible
	while (it != this->balls.end())
	{
		ball = it->second;

		if (frame->sequence - ball->sequence > 10)
		{
			delete it->second;
			it = this->balls.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Set the goal as not visible, if necessary
	if (frame->sequence - this->target.sequence > 5)
	{
		this->target.visible = false;
	}

	this->age++;
	this->lastFrame = microtime();
}

bool PseudoWorld::hasBalls() const
{
	std::map<int, PseudoWorld::Ball *>::const_iterator it = this->balls.begin();
	while (it != this->balls.end())
	{
		if (it->second->age >= PSEUDOWORLD_MIN_AGE &&
			it->second->velocity.y < BALL_VELOCITY_MAX)
		{
			// Only return true when at least one of the balls is old enough
			return true;
		}

		++it;
	}

	return false;
}

PseudoWorld::Ball *PseudoWorld::getBall(int id) const
{
	std::map<int, PseudoWorld::Ball *>::const_iterator it = this->balls.find(id);

	if (it == this->balls.end())
	{
		return NULL;
	}

	return it->second;
}

int PseudoWorld::getAge() const
{
	return this->age;
}

void PseudoWorld::readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	PseudoWorld::Ball *ball = new PseudoWorld::Ball();
	int id = 0;
	double frameDt = microtime() - this->lastFrame;

	Point2d point = {
		((blob->x1 + blob->x2) / 2.0) - (CAPT_WIDTH / 2),
		CAPT_HEIGHT - ((blob->y1 + blob->y2) / 2.0)
	};

	ball->sequence = frame->sequence;
	ball->age = 0;
	ball->radius = (abs(blob->x1 - blob->x2) + abs(blob->y1 - blob->y2)) / 2;
	ball->blob = point;

	pointToPosition(ball->pos, point);

	// Verify if in any tracking area
	std::map<int, PseudoWorld::Ball *>::iterator it = this->balls.begin();
	while (it != this->balls.end())
	{
		if (frame->sequence != it->second->sequence &&
			it->second->inTrackingRegion(*ball))
		{
			ball->age = it->second->age + 1;
			ball->velocity = {
				(RELPOS_X(ball->pos) - RELPOS_X(it->second->pos)) / frameDt,
				(RELPOS_Y(ball->pos) - RELPOS_Y(it->second->pos)) / frameDt,
			};

			// Replace the previous ball
			id = it->first;

			delete it->second;
			this->balls.erase(it);

			break;
		}

		++it;
	}

	if (!id)
	{
		id = this->ids++;
	}

	//Log::printf("ball: id=%d\tradius=%f\tangle=%f", id, ball->pos.radius, ball->pos.angle);
	//Log::printf("ball: id=%d\tvx=%f\tvy=%f", id, ball->velocity.x, ball->velocity.y);

	// This is a new ball
	this->balls[id] = ball;
}

void PseudoWorld::readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	if (blob->color != this->targetColor)
	{
		return;
	}

	// Another target of the same color from the same frame
	if (this->target.sequence == frame->sequence)
	{
		if (blob->width() / 2 <= this->target.halfwidth)
		{
			return;
		}
	}

	Point2d point = {
		((blob->x1 + blob->x2) / 2.0) - (CAPT_WIDTH / 2),
		(double) (CAPT_HEIGHT - blob->y1)
	};

	this->target.visible = true;
	this->target.sequence = frame->sequence;
	this->target.halfwidth = abs(blob->x1 - blob->x2) / 2;

	pointToPosition(this->target.pos, point);
}

bool PseudoWorld::Ball::inTrackingRegion(PseudoWorld::Ball &ball) const
{
	double distance = sqrt(pow(this->blob.x - ball.blob.x, 2) +
		pow(this->blob.y - ball.blob.y, 2));

	return distance <= this->radius * 1.8;
}

bool PseudoWorld::Ball::inTribblerRegion() const
{
	return RELPOS_Y(this->pos) < 30.0 && abs(RELPOS_X(this->pos)) <= 50.0;
}
