#include "World.h"

#define TRACKING_RADIUS 65

World::World() :
	ids(1)
{
}

World::~World()
{
}

void World::onFrame(VideoFrame *frame)
{
	std::vector<World::Ball *> balls;

	for (std::vector<VideoFrame::Blob *>::size_type i = 0; i < frame->blobs.size(); i++)
	{
		VideoFrame::Blob *blob = frame->blobs[i];

		// Do not hard-code this
		if (blob->color != VideoFrame::Blob::COLOR_BALL)
		{
			continue;
		}

		World::Ball *ball = new World::Ball();

		ball->sequence = frame->sequence;

		//ball->realx = ball->distance * sin(ball->angle);
		//ball->realy = ball->distance * cos(ball->angle);

		// Temporary
		ball->realx = (blob->x1 + blob->x2) / 2;
		ball->realy = (blob->y1 + blob->y2) / 2;

		// Verify if in any tracking area
		for (std::vector<World::Ball *>::size_type j = 0; j < this->balls.size(); j++)
		{
			if (frame->sequence != this->balls[j]->sequence &&
				this->balls[j]->inTrackingBox(ball))
			{
				ball->id = this->balls[j]->id;

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
	}

	std::vector<World::Ball *>::size_type i = 0;
	while (i < this->balls.size())
	{
		if (this->balls[i]->sequence != frame->sequence)
		{
			// This ball is no longer in the frame
			this->balls.erase(this->balls.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

bool World::Ball::inTrackingBox(Ball *ball)
{
	int distance = sqrt(pow(ball->realx - this->realx, 2) + pow(ball->realy - this->realy, 2));

	return distance <= TRACKING_RADIUS;
}
