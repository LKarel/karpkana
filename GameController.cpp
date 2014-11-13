#include "GameController.h"

#define ROTATION_TIME 0.25

#define STAGE_CALL_INIT 1
#define STAGE_CALL_EXIT 2
#define STAGE_CALL_TICK 3

#define STAGE_SEARCH 1
#define STAGE_APPROACH 2
#define STAGE_TARGET 3
#define STAGE_KICK 4
#define STAGE_MAX 4

typedef struct
{
	int ball;
	PID *pid;
} ApproachState;

GameController::GameController(VideoProcessor *vp) :
	vp(vp),
	isRunning(true),
	robot("ttyACM"),
	stage(0),
	stageState(NULL)
{
	if (env_is("C22_TARGET", "blue"))
	{
		this->world.targetColor = VideoFrame::Blob::COLOR_BLUE;
	}
	else
	{
		this->world.targetColor = VideoFrame::Blob::COLOR_YELLOW;
		Log::printf("GameController: target color not specified, assuming yellow");
	}

	this->thread = std::thread(&GameController::run, this);
}

GameController::~GameController()
{
	this->stop();
}

void GameController::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread.join();
}

void GameController::run()
{
	this->gotoStage(STAGE_SEARCH);

	while (this->isRunning)
	{
		long begin = microtime();
		VideoFrame *frame = this->vp->getFrame();

		if (!frame)
		{
			usleep(5000);
			continue;
		}

		this->world.onFrame(frame);

		if (this->stage)
		{
			this->stageCall(STAGE_CALL_TICK, this->stage, this->stageState);
		}

		DebugLink::instance().fps(DebugLink::FPS_CTRL, 1000000.0 / (microtime() - begin));

		delete frame;
	}

	if (this->stage)
	{
		this->stageCall(STAGE_CALL_EXIT, this->stage, this->stageState);
	}
}

void GameController::gotoStage(int stage)
{
	if (this->stage)
	{
		this->stageCall(STAGE_CALL_EXIT, this->stage, this->stageState);
	}

	this->stage = stage;

	if (this->stage > STAGE_MAX)
	{
		this->stage = 1;
	}

	Log::printf("GameController: switching to stage %d", this->stage);

	this->stageState = this->stageCall(STAGE_CALL_INIT, this->stage, NULL);
}

void GameController::nextStage()
{
	this->gotoStage(this->stage + 1);
}

void *GameController::stageCall(int call, int stage, void *state)
{
	switch (stage)
	{
		case STAGE_SEARCH:
			return this->stageSearch(call, state);

		case STAGE_APPROACH:
			return this->stageApproach(call, state);
	}

	return NULL;
}

void *GameController::stageSearch(int call, void *state_)
{
	if (call == STAGE_CALL_TICK)
	{
		if (this->world.hasBalls())
		{
			this->nextStage();
			return NULL;
		}

		// TODO: Do something useful
	}

	return NULL;
}

void *GameController::stageApproach(int call, void *state_)
{
	ApproachState *state = (ApproachState *) state_;

	if (call == STAGE_CALL_INIT)
	{
		state = (ApproachState *) malloc(sizeof(ApproachState));
		state->ball = 0;
		state->pid = new PID(1.0, 0.125, 0.3);

		return state;
	}
	else if (call == STAGE_CALL_EXIT)
	{
		free(state->pid);
		free(state);

		return NULL;
	}

	// TODO: Check here, if ball caught

	PseudoWorld::Ball *ball = this->world.getBall(state->ball);

	if (!ball)
	{
		if (state->ball)
		{
			Log::printf("GameController: STAGE_APPROACH: lost ball tracking");
		}

		state->ball = this->chooseBall();

		if (!state->ball)
		{
			// Could not find any balls to approach
			this->gotoStage(STAGE_SEARCH);
		}

		// If a ball was successfuly chosen, continue at next tick
		return NULL;
	}

	double rotate = state->pid->update(ball->pos.angle);
	int speed = rotate < 0.0 ? -30 : 30;

	if (ABS_F(rotate) < 0.16)
	{
		this->robot.direction(DIRECTION_FWD, 55);
	}
	else
	{
		this->robot.rotate(speed);
	}

	return NULL;
}

int GameController::chooseBall()
{
	double distance = 99999.0;
	int id = 0;

	std::map<int, PseudoWorld::Ball *>::iterator it = this->world.balls.begin();
	PseudoWorld::Ball *ball;

	while (it != this->world.balls.end())
	{
		ball = it->second;

		if (ball->age > PSEUDOWORLD_MIN_AGE && ball->pos.radius < distance)
		{
			distance = ball->pos.radius;
			id = it->first;
		}

		++it;
	}

	return id;
}