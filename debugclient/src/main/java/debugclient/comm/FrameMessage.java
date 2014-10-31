package debugclient.comm;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;

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

	public BufferedImage toBufferedImage()
	{
		BufferedImage buf = new BufferedImage(image.getWidth(null), image.getHeight(null),
			BufferedImage.TYPE_INT_ARGB);

		Graphics2D g = buf.createGraphics();
		g.drawImage(image, 0, 0, null);
		g.dispose();

		return buf;
	}
}
