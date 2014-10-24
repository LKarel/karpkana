package debugclient.comm;

import java.awt.Image;

public class FrameMessage implements Message
{
	public final int sequence;
	public final float scale;
	public final Image image;

	public FrameMessage(int sequence, float scale, Image image)
	{
		this.sequence = sequence;
		this.scale = scale;
		this.image = image;
	}
}
