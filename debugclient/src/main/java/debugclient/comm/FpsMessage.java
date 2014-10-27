package debugclient.comm;

public class FpsMessage implements Message
{
	public static final int TYPE_CAPTURE = 0x01;
	public static final int TYPE_PROC = 0x02;
	public static final int TYPE_CTRL = 0x03;

	public final int type;
	public final int fps;

	public FpsMessage (int type, int fps)
	{
		this.type = type;
		this.fps = fps;
	}
}
