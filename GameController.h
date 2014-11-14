#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <thread>
#include <unistd.h>
#include "util.h"
#include "PID.h"
#include "Robot.h"
#include "PseudoWorld.h"
#include "comm/Log.h"
#include "comm/Motor.h"
#include "vp/VideoFrame.h"
#include "vp/VideoProcessor.h"

class GameController
{
public:
	GameController(VideoProcessor *vp);
	~GameController();

	void start();
	void stop();

private:
	VideoProcessor *vp;
	bool isRunning;
	std::thread thread;

	Robot robot;
	PseudoWorld world;

	int stage;
	void *stageState;

	void run();

	void gotoStage(int stage);
	void nextStage();
	void *stageCall(int call, int stage, void *state);
	void *stageSearch(int call, void *state_);
	void *stageApproach(int call, void *state_);

	int chooseBall();
};

#endif
