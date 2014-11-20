#include <signal.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "util.h"
#include "DebugController.h"
#include "GameController.h"
#include "Kbui.h"
#include "comm/DebugLink.h"
#include "comm/Log.h"
#include "vp/Camera.h"
#include "vp/ImageCamera.h"
#include "vp/VideoProcessor.h"

#define COLORS_FILE "config/colors.txt"
#define INOTIFY_BUF_SIZE (sizeof(struct inotify_event) + 16)

bool sigint = false;

void handleSigint(int signal)
{
	if (sigint)
	{
		kill(getpid(), SIGTERM);
	}

	sigint = true;
}

int main(int argc, char** argv)
{
	signal(SIGINT, handleSigint);
	signal(SIGPIPE, SIG_IGN);

	setbuf(stdout, NULL);

	BaseCamera *camera;

	if (env_has("C22_SOURCE"))
	{
		camera = (BaseCamera *) new ImageCamera(getenv("C22_SOURCE"));
	}
	else
	{
		const char *camdev = "/dev/video0";

		if (env_has("C22_CAMDEV"))
		{
			camdev = getenv("C22_CAMDEV");
		}

		camera = (BaseCamera *) new Camera(camdev, CAPT_WIDTH, CAPT_HEIGHT);
	}

	VideoProcessor *vp = new VideoProcessor();
	vp->loadColors(COLORS_FILE);

	int inotifyFd = inotify_init1(IN_NONBLOCK);
	int inotifyWd = -1;

	if (inotifyFd >= 0)
	{
		inotifyWd = inotify_add_watch(inotifyFd, "config/", IN_MODIFY);
	}
	else
	{
		Log::perror("main: initializing inotify");
	}

	if (env_is("C22_DEBUGMODE", "1"))
	{
		DebugController ctrl(vp);

		while (!sigint)
		{
			camera->Update();
			vp->putRawFrame(camera->data);
		}

		ctrl.stop();
	}
	else
	{
		GameController ctrl(vp);
		Kbui kbui(0);

		while (!sigint)
		{
			camera->Update();
			vp->putRawFrame(camera->data);

			switch (kbui.cmd())
			{
				case Kbui::CMD_BEGIN:
					ctrl.start();
				break;

				case Kbui::CMD_STOP:
					ctrl.stop();
				break;
			}

			char buf[INOTIFY_BUF_SIZE];
			ssize_t size = read(inotifyFd, &buf, INOTIFY_BUF_SIZE);

			if (size > 0)
			{
				struct inotify_event *event = (struct inotify_event*) &buf;

				if (strcmp(event->name, "colors.txt") == 0)
				{
					Log::printf("main: colors config changed, reloading");
					vp->loadColors(COLORS_FILE);
				}
			}
		}

		ctrl.stop();
		kbui.stop();
	}

	Log::printf("main: shutting down");

	if (inotifyFd >= 0)
	{
		inotify_rm_watch(inotifyFd, inotifyWd);
		close(inotifyFd);
	}

	delete vp;
	delete camera;

	return 0;
}
