#include "comm/DebugLink.h"

DebugLink::DebugLink() :
	isRunning(true),
	frameSequence(0),
	imageData(NULL)
{
	this->server = new DebugServer();
	this->server->start(11000);

	this->thread = std::thread(&DebugLink::run, this);
}

DebugLink::~DebugLink()
{
	delete this->server;
}

DebugLink &DebugLink::instance()
{
	static DebugLink instance;
	return instance;
}

void DebugLink::close()
{
	this->isRunning = false;
	this->thread.join();

	this->server->stop();
}

void DebugLink::image(int sequence, rgb *img)
{
	if (sequence < this->frameSequence)
	{
		return;
	}

	if (this->imageMutex.try_lock())
	{
		if (this->imageData)
		{
			delete this->imageData;
		}

		this->imageData = img;
		this->imageMutex.unlock();
	}
}

void DebugLink::fps(uint8_t type, int fps)
{
	this->fpsData.push_back(fps);

	if (this->fpsData.size() == DEBUGLINK_FPS_SAMPLES)
	{
		int sum = 0;
		int fps;

		for (std::vector<int>::size_type i = 0; i < DEBUGLINK_FPS_SAMPLES; i++)
		{
			sum += this->fpsData[i];
		}

		fps = sum / DEBUGLINK_FPS_SAMPLES;
		fps = (fps > 255) ? 255 : fps;

		uint8_t data[] = {
			DebugLink::PROTOCOL_TYPE_FPS,
			type,
			(uint8_t) (fps & 0xFF)
		};

		this->server->broadcast(data, 3);
		this->fpsData.clear();
	}
}

void DebugLink::run()
{
	while (this->isRunning)
	{
		broadcastImage();
		usleep(10000);
	}
}

void DebugLink::broadcastImage()
{
	uint8_t *buf;

	{
		std::lock_guard<std::mutex> lock(this->imageMutex);

		if (!this->imageData)
		{
			return;
		}

		buf = (uint8_t *) malloc(CAPT_WIDTH * CAPT_HEIGHT * 3);

		for (size_t i = 0; i < (CAPT_WIDTH * CAPT_HEIGHT); i++)
		{
			buf[(i * 3)] = this->imageData[i].red;
			buf[(i * 3) + 1] = this->imageData[i].green;
			buf[(i * 3) + 2] = this->imageData[i].blue;
		}
	}

	int jpegSize = CAPT_WIDTH * CAPT_HEIGHT;
	uint8_t *jpeg = (uint8_t *) malloc(jpegSize);

	jpge::compress_image_to_jpeg_file_in_memory(jpeg, jpegSize,
		CAPT_WIDTH, CAPT_HEIGHT, 3, buf);

	jpge::compress_image_to_jpeg_file("/tmp/e.jpg", CAPT_WIDTH, CAPT_HEIGHT, 3, buf);

	uint8_t header[] = {
		DebugLink::PROTOCOL_TYPE_IMAGE,

		// Frame frameSequence number
		(uint8_t) ((this->frameSequence >> 24) & 0xFF),
		(uint8_t) ((this->frameSequence >> 16) & 0xFF),
		(uint8_t) ((this->frameSequence >> 8) & 0xFF),
		(uint8_t) (this->frameSequence & 0xFF),

		// Frame height
		(CAPT_HEIGHT >> 8) & 0xFF,
		(CAPT_HEIGHT) & 0xFF,

		// Image width
		(CAPT_WIDTH >> 8) & 0xFF,
		(CAPT_WIDTH) & 0xFF,

		// Image height
		(CAPT_HEIGHT >> 8) & 0xFF,
		(CAPT_HEIGHT) & 0xFF,

		(uint8_t) ((jpegSize >> 24) & 0xFF),
		(uint8_t) ((jpegSize >> 16) & 0xFF),
		(uint8_t) ((jpegSize >> 8) & 0xFF),
		(uint8_t) (jpegSize & 0xFF)
	};

	this->server->broadcast(header, 15);
	this->server->broadcast(jpeg, jpegSize);

	free(buf);
	free(jpeg);
}

// For reference
//void DebugLink::localMsg(int level, const std::string message)
//{
	//std::string levelStr;

	//switch (level)
	//{
		//case DebugLink::LEVEL_INFO: levelStr = "INFO"; break;
		//case DebugLink::LEVEL_WARN: levelStr = "WARN"; break;
		//case DebugLink::LEVEL_ERROR: levelStr = "ERROR"; break;
		//default: levelStr = "DEBUG";
	//}

	//char log[2048];
	//snprintf(log, 2048, "[%llu] %s: %s", microtime() / 1000, levelStr.c_str(), message.c_str());

	//if (this->logFile.is_open())
	//{
		//this->logFile << log << "\n";
	//}

	//std::cout << log << std::endl;
//}
