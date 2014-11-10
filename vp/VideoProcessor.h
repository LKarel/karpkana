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
	 * Load the colors config file.
	 */
	void loadColors(const char *file);

	/**
	 * Insert a camera frame for processing
	 */
	void putRawFrame(unsigned char *data);

	/**
	 * Get a processed frame.
	 */
	VideoFrame *getFrame();

	int debugImgMode;
	bool debugBlobs;

private:
	unsigned int sequence;
	CMVision vision;
	std::mutex visionMutex;

	unsigned char data[CAPT_WIDTH * CAPT_HEIGHT * 4];
	bool dataFresh;
	std::mutex dataMutex;
};

#endif
