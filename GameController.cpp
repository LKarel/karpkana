#include "GameController.h"

#define ROTATION_TIME 0.25

#define STAGE_CALL_INIT 1
#define STAGE_CALL_EXIT 2
#define STAGE_CALL_TICK 3

#define STAGE_SEARCH 1
#define STAGE_APPROACH 2
#define STAGE_TARGET 3
#define STAGE_KICK 4
#define STAGE_CYCLE_END 5
#define STAGE_IDLE 6
#define STAGE_UNSTALL 7

typedef struct
{
	int ball;
	PID *pid;
} ApproachState;

typedef struct
{
	PID *pid;
} TargetState;

GameController::GameController(VideoProcessor *vp) :
	vp(vp),
	isRunning(false),
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
}

GameController::~GameController()
{
	this->stop();
}

void GameController::start()
{
	if (!this->isRunning)
	{
		Log::printf("GameController: starting");

		this->isRunning = true;
		this->thread = std::thread(&GameController::run, this);
	}
}

void GameController::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	Log::printf("GameController: stopping");

	this->isRunning = false;
	this->thread.join();
}

void GameController::run()
{
	long begin = 0;

	this->stage = 0;
	this->stageState = NULL;

	this->gotoStage(STAGE_SEARCH);

	while (this->isRunning)
	{
		if (!begin)
		{
			begin = microtime();
		}

		VideoFrame *frame = this->vp->getFrame();

		if (!frame)
		{
			usleep(5000);
			continue;
		}

		this->world.onFrame(frame);

		if (this->stage != STAGE_UNSTALL && this->robot.getStall())
		{
			this->gotoStage(STAGE_UNSTALL);
		}

		if (this->stage)
		{
			this->stageCall(STAGE_CALL_TICK, this->stage, this->stageState);
		}

		DebugLink::instance().fps(DebugLink::FPS_CTRL, 1000000.0 / (microtime() - begin));
		begin = 0;

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
		this->robot.stop();
	}

	this->stage = stage;

	if (this->stage == STAGE_CYCLE_END)
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

		case STAGE_TARGET:
			return this->stageTarget(call, state);

		case STAGE_KICK:
			return this->stageKick(call, state);

		case STAGE_IDLE:
			return this->stageIdle(call, state);

		case STAGE_UNSTALL:
			return this->stageUnstall(call, state);
	}

	return NULL;
}

void *GameController::stageSearch(int call, void *state_)
{
	if (call == STAGE_CALL_INIT)
	{
		this->robot.rotate(25);
		return NULL;
	}
	else if (call == STAGE_CALL_EXIT)
	{
		return NULL;
	}

	if (this->world.hasBalls())
	{
		this->nextStage();
	}
	else
	{
		usleep(1000);
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
		state->pid = new PID(1.0, 0.05, 0.3);

		this->robot.coilgun->tribbler(true);

		return state;
	}
	else if (call == STAGE_CALL_EXIT)
	{
		free(state->pid);
		free(state);

		return NULL;
	}

	if (this->robot.motors[MOTOR_BDET]->queryBall())
	{
		Log::printf("GameController: STAGE_APPROACH: caught ball");
		usleep(1000);
		this->nextStage();
		return NULL;
	}

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


	//double rotate = state->pid->update(-ball->pos.angle);
	double rotate = -ball->pos.angle;
	int rotateSpeed = LIMIT(speedForRotation(-rotate, 0.2), -65, 65);

	printf("rotate=%f\trotateSpeed=%d\n", rotate, rotateSpeed);

	if (ABS_F(rotate) < 0.2)
	{
		this->robot.direction(DIRECTION_FWD, 40);
	}
	else
	{
		this->robot.rotate(rotateSpeed);
	}

	//this->robot.rotateForward(35, rotateSpeed);

	return NULL;
}

void *GameController::stageTarget(int call, void *state_)
{
	TargetState * state = (TargetState *) state_;

	if (call == STAGE_CALL_INIT)
	{
		state = (TargetState *) malloc(sizeof(TargetState));
		state->pid = new PID(1.0, 0.05, 0.3);

		this->robot.coilgun->tribbler(true);

		return state;
	}
	else if (call == STAGE_CALL_EXIT)
	{
		free(state->pid);
		free(state);

		this->robot.coilgun->tribbler(false);

		return NULL;
	}

	double angle = this->world.target.pos.angle;

	if (!this->world.target.visible)
	{
		angle = PI / -3.0;
	}

	int rotateSpeed = LIMIT(speedForRotation(-angle, 0.3), -65, 65);

	printf("angle=%f\trotateSpeed=%d\n", angle, rotateSpeed);

	if (ABS_F(angle) < 0.2)
	{
		this->nextStage();
	}
	else
	{
		this->robot.rotate(rotateSpeed);
	}

	return NULL;
}

void *GameController::stageKick(int call, void *state_)
{
	if (call == STAGE_CALL_TICK)
	{
		this->robot.coilgun->kick(20000);
		usleep(50 * 1000);

		this->nextStage();
	}

	return NULL;
}

void *GameController::stageIdle(int call, void *state_)
{
	return NULL;
}

void *GameController::stageUnstall(int call, void *state_)
{
	if (call == STAGE_CALL_INIT)
	{
		this->robot.stop();
		this->robot.coilgun->tribbler(false);
	}
	else if (call == STAGE_CALL_EXIT)
	{
		return NULL;
	}

	if (!this->robot.getStall())
	{
		this->gotoStage(STAGE_SEARCH);
	}
	else
	{
		usleep(10000);
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
