#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "comm/Coilgun.h"
#include "comm/DebugLink.h"
#include "comm/Log.h"
#include "vp/Camera.h"
#include "vp/VideoProcessor.h"

bool sigint = false;

void handleSigint(int signal)
{
	sigint = true;
}

int main(int argc, char** argv)
{
	signal(SIGINT, handleSigint);
	signal(SIGPIPE, SIG_IGN);

	setbuf(stdout, NULL);

	Coilgun cg("/home/ragnis/tmp/cg", 3);

	while (!sigint)
	{
		cg.tick();
		usleep(10000);
	}

	return 0;

	Camera camera("/dev/video0", CAPT_WIDTH, CAPT_HEIGHT);

	VideoProcessor vp;
	vp.debugClassify = true;

	while (!sigint)
	{
		camera.Update();
		vp.putRawFrame(camera.data);
		vp.getFrame();
	}

	Log::printf("main: shutting down");
	DebugLink::instance().close();

	return 0;
}
