package debugclient;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.awt.image.WritableRaster;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import javax.imageio.ImageIO;
import javax.swing.JPanel;

import org.imgscalr.Scalr;

public class VideoPanel extends JPanel
{
	public enum Channel {NONE, Y, U, V};

	private static final int WIDTH = 640;
	private static final int HEIGHT = 480;

	private BufferedImage mImage = null;

	private Channel mThresholdChannel = Channel.NONE;
	private int mThresholdLow = 0;
	private int mThresholdHigh = 0;

	public VideoPanel()
	{
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
	}

	public void putFrame(c22dlink.FrameImage msg)
	{
		try
		{
			mImage = ImageIO.read(msg.getImage().newInput());
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}

		repaint();
	}

	public void setThreshold(Channel channel)
	{
		mThresholdChannel = channel;
	}

	public void setThreshold(Channel channel, int low, int high)
	{
		mThresholdChannel = channel;
		mThresholdLow = low;
		mThresholdHigh = high;
	}

	@Override
	public synchronized void paintComponent(Graphics graphics)
	{
		super.paintComponent(graphics);

		if (mImage == null)
		{
			return;
		}

		BufferedImage image = Scalr.resize(mImage, Scalr.Method.SPEED,
			Scalr.Mode.FIT_EXACT, WIDTH, HEIGHT, Scalr.OP_ANTIALIAS);

		if (mThresholdChannel != Channel.NONE)
		{
			int[] rgb = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
			byte[] out = new byte[WIDTH * HEIGHT];

			int r, g, b;
			int ch;

			for (int i = 0; i < WIDTH * HEIGHT; i++)
			{
				r = (rgb[i] >> 16) & 0xFF;
				g = (rgb[i] >> 8) & 0xFF;
				b = rgb[i] & 0xFF;

				switch (mThresholdChannel)
				{
					case Y:
						ch = (int) (0.257 * r + 0.504 * g + 0.098 * b + 16.0);
						break;

					case U:
						ch = (int) (-0.148 * r - 0.291 * g + 0.439 * b + 128.0);
						break;

					case V:
						ch = (int) (0.439 * r - 0.368 * g - 0.071 * b + 128.0);
						break;

					default:
						continue;
				}

				ch = (ch < 0) ? 0 : ((ch > 255) ? 255 : ch);
				out[i] = (ch >= mThresholdLow && ch <= mThresholdHigh) ? (byte) 0xFF : 0;
			}

			image = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_BYTE_GRAY);
			image.getRaster().setDataElements(0, 0, WIDTH, HEIGHT, out);
		}

		graphics.drawImage((Image) image, 0, 0, null);
	}
}
