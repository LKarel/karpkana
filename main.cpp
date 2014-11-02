#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"
#include "TestController.h"
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

	Camera camera("/dev/video0", CAPT_WIDTH, CAPT_HEIGHT);

	VideoProcessor vp;
	TestController ctrl(vp);

	if (env_is("C22_DEBUGIMG", "raw"))
	{
		vp.debugImgMode = VideoProcessor::DEBUG_IMG_RAW;
	}
	else if (env_is("C22_DEBUGIMG", "classify"))
	{
		vp.debugImgMode = VideoProcessor::DEBUG_IMG_CLASSIFY;
	}

	while (!sigint)
	{
		camera.Update();
		vp.putRawFrame(camera.data);
	}

	Log::printf("main: shutting down");
	ctrl.stop();
	DebugLink::instance().close();

	return 0;
}
