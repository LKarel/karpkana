#include "PseudoWorld.h"

PseudoWorld::PseudoWorld() :
	targetColor(VideoFrame::Blob::COLOR_YELLOW),
	ids(1)
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
			// TODO: If in tribbler area and acceptable velocity...

			delete it->second;
			this->balls.erase(it);
		}

		++it;
	}

	// Set the goal as not visible, if necessary
	if (frame->sequence - this->target.sequence > 5)
	{
		this->target.visible = false;
	}
}

bool PseudoWorld::hasBalls() const
{
	std::map<int, PseudoWorld::Ball *>::const_iterator it = this->balls.begin();
	while (it != this->balls.end())
	{
		if (it->second->age >= PSEUDOWORLD_MIN_AGE)
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

void PseudoWorld::readBallBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	PseudoWorld::Ball *ball = new PseudoWorld::Ball();
	int id = 0;

	Point2d point = {
		((blob->x1 + blob->x2) / 2.0) - (CAPT_WIDTH / 2),
		CAPT_HEIGHT - ((blob->y1 + blob->y2) / 2.0)
	};

	ball->sequence = frame->sequence;
	ball->age = 0;
	ball->radius = (abs(blob->x1 - blob->x2) + abs(blob->y1 - blob->y2)) / 2;
	ball->pos = { sqrt(pow(point.x, 2) + pow(point.y, 2)), atan(point.x / point.y) };
	ball->velocity = {0};

	// Verify if in any tracking area
	std::map<int, PseudoWorld::Ball *>::iterator it = this->balls.begin();
	while (it != this->balls.end())
	{
		if (frame->sequence != it->second->sequence &&
			it->second->inTrackingRegion(*ball))
		{
			ball->age = it->second->age + 1;
			ball->velocity = {
				RELPOS_X(it->second->pos) - RELPOS_X(ball->pos),
				RELPOS_Y(it->second->pos) - RELPOS_Y(ball->pos),
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

	// This is a new ball
	this->balls[id] = ball;
}

void PseudoWorld::readGoalBlob(VideoFrame *frame, VideoFrame::Blob *blob)
{
	if (blob->color != this->targetColor)
	{
		return;
	}

	Point2d point = {
		((blob->x1 + blob->x2) / 2.0) - (CAPT_WIDTH / 2),
		(double) (CAPT_HEIGHT - blob->y1)
	};

	this->target.visible = true;
	this->target.sequence = frame->sequence;
	this->target.halfwidth = abs(blob->x1 - blob->x2) / 2;
	this->target.pos = {
		sqrt(pow(point.x, 2) + pow(point.y, 2)),
		atan(point.x / point.y)
	};
}

bool PseudoWorld::Ball::inTrackingRegion(PseudoWorld::Ball &ball) const
{
	return relPositionDistance(this->pos, ball.pos) <= this->radius * 1.5;
}

bool PseudoWorld::Ball::inTribblerRegion() const
{
	return RELPOS_Y(this->pos) < 30 && abs(RELPOS_X(this->pos)) <= 40;
}
