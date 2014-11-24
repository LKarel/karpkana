#include "VideoProcessor.h"

#define VP_IGNORE_TOP 50
#define VP_MIN_HEIGHT 7
#define VP_MIN_WIDTH 7
#define VP_BLACK_STEP 2
#define VP_BLACK_THRESHOLD 4

VideoProcessor::VideoProcessor() :
	keepOriginal(false),
	keepClassify(false),
	sequence(0),
	dataFresh(false)
{
	this->vision.initialize(CAPT_WIDTH, CAPT_HEIGHT);
	this->vision.enable(CMV_DENSITY_MERGE);
}

VideoProcessor::~VideoProcessor()
{
	this->vision.close();
}

CMVision *VideoProcessor::getVision()
{
	return &this->vision;
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

	for (size_t color = 0; color < CMV_MAX_COLORS; color++)
	{
		CMVision::region *region = this->vision.getRegions(color);

		for(; region; region = region->next)
		{
			double width = abs(region->x1 - region->x2);
			double height = abs(region->y1 - region->y2);

			double ratio = width / height;
			double density = region->area / (width * height);

			if (width < VP_MIN_WIDTH || height < VP_MIN_HEIGHT || region->cen_y < VP_IGNORE_TOP)
			{
				continue;
			}

			if (region->color == VideoFrame::Blob::COLOR_BALL)
			{
				if (ratio < 0.3 || ratio > 1.7 || density < 0.5)
				{
					// Not a valid ball
					continue;
				}

				double ascent = (region->cen_x - (CAPT_WIDTH / 2)) / (CAPT_HEIGHT - region->cen_y);
				int blackCount = 0;

				for (size_t n = 0; blackCount < VP_BLACK_STEP; ++n)
				{
					int testX = region->cen_x - VP_BLACK_STEP * n * ascent;
					int testY = region->cen_y + VP_BLACK_STEP * n;
					int mapIndex = (CAPT_WIDTH * testY) + testX;

					if (mapIndex > CAPT_WIDTH * CAPT_HEIGHT)
					{
						break;
					}

					if (this->vision.getMap()[mapIndex] & (1 << VideoFrame::Blob::COLOR_BLACK))
					{
						++blackCount;
					}
				}

				if (blackCount >= VP_BLACK_STEP)
				{
					continue;
				}
			}

			if (region->color == VideoFrame::Blob::COLOR_YELLOW ||
				region->color == VideoFrame::Blob::COLOR_BLUE)
			{
				if (width < 75 || height < 10 || density < 0.3)
				{
					// Not a valid goal
					continue;
				}
			}

			vf->blobs.push_back(VideoFrame::Blob::fromRegion(region));
		}
	}

	if (this->keepClassify)
	{
		vf->imageClassify = new rgb[CAPT_WIDTH * CAPT_HEIGHT];
		this->vision.testClassify(vf->imageClassify, cmImg);
	}

	if (this->keepOriginal)
	{
		vf->imageOriginal = new rgb[CAPT_WIDTH * CAPT_HEIGHT];
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

			vf->imageOriginal[i].red = (unsigned char) LIMIT(r, 0, 255);
			vf->imageOriginal[i].green = (unsigned char) LIMIT(g, 0, 255);
			vf->imageOriginal[i].blue = (unsigned char) LIMIT(b, 0, 255);
		}
	}

	delete[] cmImg;

	return vf;
}

