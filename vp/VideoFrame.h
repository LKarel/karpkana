#ifndef VP_VIDEOFRAME_H
#define VP_VIDEOFRAME_H

#include <vector>
#include "main.h"
#include "util.h"
#include "3rdparty/cmvision.h"

class VideoFrame
{
public:
	class Blob
	{
	public:
		static const int COLOR_BALL = 0;
		static const int COLOR_BLUE = 1;
		static const int COLOR_YELLOW = 2;
		static const int COLOR_BLACK = 3;

		static Blob *fromRegion(CMVision::region *region);

		Blob(int x1, int y1, int x2, int y2, int color);

		int x1;
		int x2;
		int y1;
		int y2;
		int color;

		int width();
		int height();
	};

	VideoFrame(int sequence);
	~VideoFrame();

	int sequence;
	double darkDensity;
	std::vector<VideoFrame::Blob *> blobs;

	rgb *imageOriginal;
	rgb *imageClassify;

};

#endif
