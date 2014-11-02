#ifndef VP_VIDEOPROCESSOR_H
#define VP_VIDEOPROCESSOR_H

#include <mutex>
#include <thread>
#include "main.h"
#include "util.h"
#include "3rdparty/cmvision.h"
#include "comm/DebugLink.h"
#include "vp/Camera.h"
#include "vp/VideoFrame.h"

class VideoProcessor
{
public:
	static const int DEBUG_IMG_NONE = 0;
	static const int DEBUG_IMG_RAW = 1;
	static const int DEBUG_IMG_CLASSIFY = 2;

	VideoProcessor();
	~VideoProcessor();

	/**
	 * Insert a camera frame for processing
	 */
	void putRawFrame(unsigned char *data);

	/**
	 * Get a processed frame.
	 */
	VideoFrame *getFrame();

	int debugImgMode;

private:
	unsigned int sequence;
	CMVision vision;

	unsigned char data[CAPT_WIDTH * CAPT_HEIGHT * 4];
	std::mutex dataMutex;
};

#endif
