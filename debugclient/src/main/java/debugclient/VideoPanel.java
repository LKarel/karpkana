package debugclient;

import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;

import javax.swing.JPanel;

public class VideoPanel extends JPanel
{
	private static final int WIDTH = 480;
	private static final int HEIGHT = 360;

	private Image mFrame;
	private String mErrorMessage = "NO SIGNAL";

	private Image mNoise;
	private Timer mNoiseTimer;

	public VideoPanel()
	{
		setPreferredSize(new Dimension(WIDTH, HEIGHT));

		mNoiseTimer = new Timer();
		mNoiseTimer.schedule(new TimerTask()
		{
			public void run()
			{
				Random random = new Random();
				int[] pixels = new int[WIDTH * HEIGHT];
				int component;

				for (int i = 0; i < (WIDTH / 4) * HEIGHT; i++)
				{
					component = random.nextInt(148);
					pixels[i * 4] = component;
					pixels[(i * 4) + 1] = component;
					pixels[(i * 4) + 2] = component;
					pixels[(i * 4) + 3] = component;
				}

				BufferedImage bufimg = new BufferedImage(WIDTH, HEIGHT,
					BufferedImage.TYPE_BYTE_GRAY);
				WritableRaster raster = bufimg.getRaster();
				raster.setPixels(0, 0, WIDTH, HEIGHT, pixels);

				mNoise = (Image) bufimg;
				repaint();
			}
		}, 0, 75);
	}

	public void setFrame(Image frame)
	{
		mFrame = frame;
		mErrorMessage = null;
		mNoiseTimer.cancel();
		repaint();
	}

	public void setErrorMessage(String msg)
	{
		mErrorMessage = msg;
		repaint();
	}

	@Override
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		Image image = mFrame;
		int width = WIDTH;
		int height = HEIGHT;

		if (mFrame == null)
		{
			if (mNoise == null)
			{
				return;
			}

			image = mNoise;
		}
		else
		{
			width = image.getWidth(null);
			height = image.getHeight(null);
		}

		g.drawImage(image, 0, 0, width, height, 0, 0, width, height, null);

		if (mErrorMessage != null)
		{
			paintError(g, width, height, mErrorMessage);
		}
	}

	protected void paintError(Graphics g, int width, int height, String msg)
	{
		Font font = new Font("Sans-serif", Font.BOLD, 38);
		FontMetrics fontMetrics = g.getFontMetrics(font);
		int textWidth = fontMetrics.stringWidth(msg);

		g.setFont(font);
		g.setColor(new Color(1.0f, 0.0f, 0.0f, 0.8f));
		g.drawString(msg, (width - textWidth) / 2, (height / 2) + 19);
	}
}
