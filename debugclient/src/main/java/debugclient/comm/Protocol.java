package debugclient.comm;

import com.google.protobuf.Message;

public class Protocol
{
	public static final int TYPE_BALL = 0x2;
	public static final int TYPE_IMAGE = 0x3;
	public static final int TYPE_BLOB = 0x4;
	public static final int TYPE_FPS = 0xA;

	public static final int FRAME_IMAGE = 0x10;
	public static final int REQUEST_COLORS = 0x20;
	public static final int COLORS_INFO = 0x21;
	public static final int COLOR_INFO = 0x22;
	public static final int LOAD_COLORS = 0x25;
	public static final int SAVE_COLORS = 0x26;

	public static int typeForMessage(Message msg)
	{
		String name = msg.getDescriptorForType().getFullName();

		if (name.equals("c22dlink.RequestColors"))
		{
			return REQUEST_COLORS;
		}
		else if (name.equals("c22dlink.ColorInfo"))
		{
			return COLOR_INFO;
		}
		else if (name.equals("c22dlink.LoadColors"))
		{
			return LOAD_COLORS;
		}
		else if (name.equals("c22dlink.SaveColors"))
		{
			return SAVE_COLORS;
		}

		return 0;
	}
}
