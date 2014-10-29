#include "VideoProcessor.h"

VideoProcessor::VideoProcessor() :
	debugClassify(false),
	sequence(0)
{
	this->vision.initialize(CAPT_WIDTH, CAPT_HEIGHT);
	this->vision.loadOptions("config/colors.txt");
}

VideoProcessor::~VideoProcessor()
{
	this->vision.close();
}

void VideoProcessor::putRawFrame(unsigned char *data)
{
	std::lock_guard<std::mutex> lock(this->dataMutex);
	memcpy(&this->data, data, CAPT_WIDTH * CAPT_HEIGHT * 4);
}

VideoFrame *VideoProcessor::getFrame()
{
	image_pixel *cmImg = new image_pixel[(CAPT_WIDTH * CAPT_HEIGHT) / 2];
	long begin = microtime();

	// Convert data to a format acceptable to CMVision
	{
		std::lock_guard<std::mutex> lock(this->dataMutex);

		for (size_t i = 0; i < (CAPT_WIDTH * CAPT_HEIGHT) / 2; i++)
		{
			cmImg[i].y1 = this->data[(i * 4)];
			cmImg[i].u = this->data[(i * 4) + 1];
			cmImg[i].y2 = this->data[(i * 4) + 2];
			cmImg[i].v = this->data[(i * 4) + 3];
		}
	}

	if (!this->vision.processFrame(cmImg))
	{
		perror("Processing frame");
		return NULL;
	}

	VideoFrame *vf = new VideoFrame(this->sequence++);

	for (size_t color = 0; color < REGION_NUM; color++)
	{
		CMVision::region *region = this->vision.getRegions(color);

		while (region)
		{
			VideoFrame::Blob *blob = VideoFrame::Blob::fromRegion(region);

			if (blob)
			{
				vf->blobs.push_back(blob);
			}

			region = region->next;
		}
	}

	if (this->debugClassify)
	{
		rgb *debugImg = new rgb[CAPT_WIDTH * CAPT_HEIGHT];
		this->vision.testClassify(debugImg, cmImg);

		DebugLink::instance().image(vf->sequence, debugImg);
	}

	DebugLink::instance().fps(DebugLink::FPS_PROC, 1000000.0 / (microtime() - begin));

	delete cmImg;

	return vf;
}
