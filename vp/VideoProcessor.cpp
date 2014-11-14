#include "VideoProcessor.h"

#define VP_MIN_HEIGHT 10
#define VP_MIN_WIDTH 10

VideoProcessor::VideoProcessor() :
	debugImgMode(DEBUG_IMG_NONE),
	debugBlobs(false),
	sequence(0),
	dataFresh(false)
{
	this->vision.initialize(CAPT_WIDTH, CAPT_HEIGHT);
}

VideoProcessor::~VideoProcessor()
{
	this->vision.close();
}

void VideoProcessor::loadColors(const char *file)
{
	std::lock_guard<std::mutex> lock(this->visionMutex);
	this->vision.loadOptions(file);
}

void VideoProcessor::putRawFrame(unsigned char *data)
{
	std::lock_guard<std::mutex> lock(this->dataMutex);
	memcpy(&this->data, data, CAPT_WIDTH * CAPT_HEIGHT * 4);
	this->dataFresh = true;
}

VideoFrame *VideoProcessor::getFrame()
{
	std::lock_guard<std::mutex> lock(this->visionMutex);

	image_pixel *cmImg;
	long begin = microtime();

	// Convert data to a format acceptable to CMVision
	{
		std::lock_guard<std::mutex> lock(this->dataMutex);

		if (!this->dataFresh)
		{
			return NULL;
		}

		cmImg = new image_pixel[(CAPT_WIDTH * CAPT_HEIGHT) / 2];

		for (size_t i = 0; i < (CAPT_WIDTH * CAPT_HEIGHT) / 2; i++)
		{
			cmImg[i].y1 = this->data[(i * 4)];
			cmImg[i].u = this->data[(i * 4) + 1];
			cmImg[i].y2 = this->data[(i * 4) + 2];
			cmImg[i].v = this->data[(i * 4) + 3];
		}

		this->dataFresh = false;
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

		for(; region; region = region->next)
		{
			if (region->color == VideoFrame::Blob::COLOR_GUIDE)
			{
				continue;
			}

			double width = abs(region->x1 - region->x2);
			double height = abs(region->y1 - region->y2);

			double ratio = width / height;
			double density = region->area / (width * height);

			if (width < VP_MIN_WIDTH || height < VP_MIN_HEIGHT || density < 0.5)
			{
				continue;
			}

			VideoFrame::Blob *blob = VideoFrame::Blob::fromRegion(region);

			if (blob->color == VideoFrame::Blob::COLOR_BALL)
			{
				if (width < 25 || ratio > 1.65 || ratio < 0.45)
				{
					// It's not a ball, maybe a discolored yellow goal?
					blob->color = VideoFrame::Blob::COLOR_YELLOW;
				}
			}

			if (blob->color == VideoFrame::Blob::COLOR_YELLOW ||
				blob->color == VideoFrame::Blob::COLOR_BLUE)
			{
				if (width < 100 || height < 10)
				{
					// Not a valid goal
					delete blob;
					continue;
				}
			}

			if (blob->color == VideoFrame::Blob::COLOR_BALL)
			{
				std::vector<VideoFrame::Blob *>::iterator it = vf->blobs.begin();
				while (it != vf->blobs.end())
				{
					if (blob->color == (*it)->color && blob->overlap(*it) > 0.75)
					{
						blob->consume(*it);

						delete *it;
						it = vf->blobs.erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			vf->blobs.push_back(blob);
		}
	}

	if (this->debugBlobs)
	{
		std::vector<VideoFrame::Blob *>::iterator it = vf->blobs.begin();
		while (it != vf->blobs.end())
		{
			DebugLink::instance().blob(vf->sequence, *it);
			++it;
		}
	}

	if (this->debugImgMode == DEBUG_IMG_CLASSIFY)
	{
		rgb *debugImg = new rgb[CAPT_WIDTH * CAPT_HEIGHT];
		this->vision.testClassify(debugImg, cmImg);

		DebugLink::instance().image(vf->sequence, debugImg);
	}
	else if (this->debugImgMode == DEBUG_IMG_RAW)
	{
		rgb *debugImg = new rgb[CAPT_WIDTH * CAPT_HEIGHT];
		image_pixel yuyv;
		int y;
		int r, g, b;

		for (size_t i = 0; i < CAPT_WIDTH * CAPT_HEIGHT; i++)
		{
			yuyv = cmImg[i / 2];

			y = (i % 2) ? yuyv.y2 : yuyv.y1;

			r = y + (1.370705 * (yuyv.v-128));
			g = y - (0.698001 * (yuyv.v-128)) - (0.337633 * (yuyv.u-128));
			b = y + (1.732446 * (yuyv.u-128));

			debugImg[i].red = (unsigned char) LIMIT(r, 0, 255);
			debugImg[i].green = (unsigned char) LIMIT(g, 0, 255);
			debugImg[i].blue = (unsigned char) LIMIT(b, 0, 255);
		}

		DebugLink::instance().image(vf->sequence, debugImg);
	}

	DebugLink::instance().fps(DebugLink::FPS_PROC, 1000000.0 / (microtime() - begin));

	delete[] cmImg;

	return vf;
}

