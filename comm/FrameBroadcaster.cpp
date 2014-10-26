#include "comm/FrameBroadcaster.h"
#include "comm/DebugLink.h"

FrameBroadcaster::FrameBroadcaster(DebugServer *server) :
	isRunning(true),
	server(server)
{
	this->thread = std::thread(&FrameBroadcaster::run, this);
}

FrameBroadcaster::~FrameBroadcaster()
{
	if (this->frame)
	{
		MM_DEC(this->frame);
	}
}

void FrameBroadcaster::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread.join();
}

void FrameBroadcaster::putFrame(Frame *frame)
{
	std::lock_guard<std::mutex> lock(this->frameMutex);

	if (this->frame || !this->server->hasClients())
	{
		return;
	}

	MM_INC(frame);
	this->frame = frame;
}

void FrameBroadcaster::run()
{
	while (this->isRunning)
	{
		usleep(1000);

		{
			std::lock_guard<std::mutex> lock(this->frameMutex);

			if (!this->frame)
			{
				continue;
			}
		}

		broadcastFrame(this->frame);

		MM_DEC(this->frame);

		std::lock_guard<std::mutex> lock(this->frameMutex);
		this->frame = NULL;
	}
}

void FrameBroadcaster::broadcastFrame(Frame *frame)
{
	int width = 640;
	int height = 360;

	cv::Mat mat;
	cv::resize(*frame->sourceMat, mat, cv::Size(width, height));

	std::vector<unsigned char> buffer;
	cv::imencode(".jpg", mat, buffer);

	uint8_t header[] = {
		DebugLink::PROTOCOL_TYPE_FRAME,

		// Frame sequence number
		(uint8_t) ((frame->sequence >> 24) & 0xFF),
		(uint8_t) ((frame->sequence >> 16) & 0xFF),
		(uint8_t) ((frame->sequence >> 8) & 0xFF),
		(uint8_t) (frame->sequence & 0xFF),

		// Frame height
		(uint8_t) ((frame->hsvMat->size().height >> 8) & 0xFF),
		(uint8_t) (frame->hsvMat->size().height & 0xFF),

		// Image width
		(uint8_t) ((width >> 8) & 0xFF),
		(uint8_t) (width & 0xFF),

		// Image height
		(uint8_t) ((height >> 8) & 0xFF),
		(uint8_t) (height & 0xFF),

		(uint8_t) ((buffer.size() >> 24) & 0xFF),
		(uint8_t) ((buffer.size() >> 16) & 0xFF),
		(uint8_t) ((buffer.size() >> 8) & 0xFF),
		(uint8_t) (buffer.size() & 0xFF)
	};

	this->server->broadcast(header, 15);
	this->server->broadcast(&buffer[0], buffer.size());

	for (std::vector<BaseObject *>::size_type i = 0; i < frame->objects.size(); ++i)
	{
		broadcastObject(frame->sequence, frame->objects[i]);
	}
}

void FrameBroadcaster::broadcastObject(int sequence, BaseObject *object)
{
	switch (object->type)
	{
		case BallObject::TYPE:
			return broadcastBallObject(sequence, (BallObject *) object);
	}
}

void FrameBroadcaster::broadcastBallObject(int sequence, BallObject *ball)
{
	uint8_t data[] = {
		DebugLink::PROTOCOL_TYPE_BALL,

		// Frame sequence number
		(uint8_t) ((sequence >> 24) & 0xFF),
		(uint8_t) ((sequence >> 16) & 0xFF),
		(uint8_t) ((sequence >> 8) & 0xFF),
		(uint8_t) (sequence & 0xFF),

		// Position x
		(uint8_t) ((ball->visual.x >> 8) & 0xFF),
		(uint8_t) (ball->visual.x & 0xFF),

		// Position y
		(uint8_t) ((ball->visual.y >> 8) & 0xFF),
		(uint8_t) (ball->visual.y & 0xFF),

		// Radius
		(uint8_t) ((ball->visual.radius >> 8) & 0xFF),
		(uint8_t) (ball->visual.radius & 0xFF)
	};

	this->server->broadcast(data, 11);
}
