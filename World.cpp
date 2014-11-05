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

		//printf("id=%d\tdistance: %d\tangle=%f\n", ball->id, ball->distance, ball->angle);
		printf("id=%d\tx=%d\ty=%d\n", ball->id, ball->realx, ball->realy);
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
