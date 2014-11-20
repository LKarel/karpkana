#include "DebugController.h"

DebugController::DebugController(VideoProcessor *vp) :
	vp(vp),
	isRunning(true)
{
	this->server.start(12000);
	this->thread = std::thread(&DebugController::run, this);

	this->vp->keepOriginal = true;
	this->vp->keepClassify = true;
}

DebugController::~DebugController()
{
	google::protobuf::ShutdownProtobufLibrary();
}

void DebugController::stop()
{
	if (this->isRunning)
	{
		this->isRunning = false;
		this->thread.join();
		this->server.stop();
	}
}

void DebugController::run()
{
	VideoFrame *frame;

	while (this->isRunning)
	{
		frame = this->vp->getFrame();

		if (!frame)
		{
			usleep(5000);
			continue;
		}

		google::protobuf::Message *incoming = NULL;
		std::string name;

		while ((incoming = this->server.getIncoming()))
		{
			name = incoming->GetTypeName();

			if (name == "c22dlink.RequestColors")
			{
				this->broadcastColorsInfo();
			}
			else if (name == "c22dlink.ColorInfo")
			{
				this->handleColorInfo((c22dlink::ColorInfo *) incoming);
			}

			delete incoming;
		}

		this->broadcastImage(c22dlink::FrameImage::ORIGINAL, frame, frame->imageOriginal);
		this->broadcastImage(c22dlink::FrameImage::CLASSIFY, frame, frame->imageClassify);

		delete frame;
	}
}

void DebugController::handleColorInfo(c22dlink::ColorInfo *msg)
{
	CMVision *vision = this->vp->getVision();
	struct CMVision::color_info *color = vision->getColorInfo(msg->id());

	if (!color || !color->name)
	{
		return;
	}

	Log::printf("DebugController: updating color '%s'", color->name);

	color->merge = msg->merge();
	color->expected_num = msg->expected();

	color->y_low = msg->yuvlow().y();
	color->u_low = msg->yuvlow().u();
	color->v_low = msg->yuvlow().v();

	color->y_high = msg->yuvhigh().y();
	color->u_high = msg->yuvhigh().u();
	color->v_high = msg->yuvhigh().v();

	vision->recalculateOptions();
}

void DebugController::broadcastColorsInfo()
{
	CMVision *vision = this->vp->getVision();
	struct CMVision::color_info *color;

	c22dlink::ColorsInfo msg;

	for (int i = 0; i < CMV_MAX_COLORS; ++i)
	{
		color = vision->getColorInfo(i);

		if (!color || !color->name)
		{
			break;
		}

		c22dlink::ColorInfo *colorInfo = msg.add_colors();
		colorInfo->set_id(i);
		colorInfo->set_name(color->name);
		colorInfo->set_merge(color->merge);
		colorInfo->set_expected(color->expected_num);

		colorInfo->mutable_rgb()->set_r(color->color.red);
		colorInfo->mutable_rgb()->set_g(color->color.green);
		colorInfo->mutable_rgb()->set_b(color->color.blue);

		colorInfo->mutable_yuvlow()->set_y(color->y_low);
		colorInfo->mutable_yuvlow()->set_u(color->u_low);
		colorInfo->mutable_yuvlow()->set_v(color->v_low);

		colorInfo->mutable_yuvhigh()->set_y(color->y_high);
		colorInfo->mutable_yuvhigh()->set_u(color->u_high);
		colorInfo->mutable_yuvhigh()->set_v(color->v_high);
	}

	this->broadcastMessage(PROTOCOL__COLORS_INFO, msg);
}

void DebugController::broadcastImage(c22dlink::FrameImage::Type type, VideoFrame *frame, rgb *image)
{
	unsigned char *buf = (unsigned char *) malloc(CAPT_WIDTH * CAPT_HEIGHT * 3);

	for (size_t i = 0; i < (CAPT_WIDTH * CAPT_HEIGHT); i++)
	{
		buf[(i * 3)] = image[i].red;
		buf[(i * 3) + 1] = image[i].green;
		buf[(i * 3) + 2] = image[i].blue;
	}

	int jpegSize = CAPT_WIDTH * CAPT_HEIGHT;
	unsigned char *jpeg = (unsigned char *) malloc(jpegSize);

	jpge::compress_image_to_jpeg_file_in_memory(jpeg, jpegSize,
		CAPT_WIDTH, CAPT_HEIGHT, 3, buf);

	c22dlink::FrameImage msg;
	msg.set_sequence(frame->sequence);
	msg.set_type(type);
	msg.set_width(CAPT_WIDTH);
	msg.set_height(CAPT_HEIGHT);
	msg.set_image((const char *) jpeg, jpegSize);

	this->broadcastMessage(PROTOCOL__FRAME_IMAGE, msg);

	free(buf);
	free(jpeg);
}

void DebugController::broadcastMessage(uint8_t type, google::protobuf::Message &msg)
{
	int len = msg.ByteSize();
	uint8_t *payload = (uint8_t *) malloc(len);

	uint8_t header[] = {
		type,

		(uint8_t) ((len >> 24) & 0xFF),
		(uint8_t) ((len >> 16) & 0xFF),
		(uint8_t) ((len >> 8) & 0xFF),
		(uint8_t) (len & 0xFF)
	};

	msg.SerializeToArray(payload, len);

	this->server.broadcast(header, 5);
	this->server.broadcast(payload, len);

	free(payload);
}
