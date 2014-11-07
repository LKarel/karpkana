#ifndef VP_BASECAMERA_H
#define VP_BASECAMERA_H

class BaseCamera
{
public:
	virtual ~BaseCamera()
	{
	}

	virtual bool Update(unsigned int t=100, int timeout_ms=500)
	{
		return true;
	}

	unsigned char *data;
};

#endif
