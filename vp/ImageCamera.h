#ifndef VP_IMAGECAMERA_H
#define VP_IMAGECAMERA_H

#include <cassert>
#include "main.h"
#include "util.h"
#include "3rdparty/jpgd.h"
#include "comm/Log.h"
#include "vp/BaseCamera.h"

class ImageCamera : public BaseCamera
{
public:
	ImageCamera(const char *path);
	~ImageCamera();
};

#endif
