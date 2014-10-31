package debugclient;

import java.util.ArrayList;
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

import org.imgscalr.Scalr;

import debugclient.comm.BallMessage;
import debugclient.comm.FrameMessage;

public class VideoPanel extends JPanel
{
	private static final int WIDTH = 746;
	private static final int HEIGHT = 480;

	private FrameMessage mFrame;
	private ArrayList<BallMessage> mBalls;
	private String mErrorMessage = "NO SIGNAL";

	private Image mNoise;
	private Timer mNoiseTimer;

	public VideoPanel()
	{
		setPreferredSize(new Dimension(WIDTH, HEIGHT));

		mBalls = new ArrayList<BallMessage>();

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
					component = random.nextInt(35);
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

	public void setFrame(FrameMessage frame)
	{
		mFrame = frame;
		mErrorMessage = null;
		mBalls.clear();

		mNoiseTimer.cancel();
		repaint();
	}

	public void putBall(BallMessage ball)
	{
		mBalls.add(ball);
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

		Image image = null;

		if (mFrame != null)
		{
			image = (Image) Scalr.resize(mFrame.toBufferedImage(), Scalr.Method.SPEED,
				Scalr.Mode.FIT_EXACT, WIDTH, HEIGHT, Scalr.OP_ANTIALIAS);
		}
		else if (mNoise != null)
		{
			image = mNoise;
		}
		else
		{
			return;
		}

		g.drawImage(image, 0, 0, null);

		if (mErrorMessage != null)
		{
			paintError(g, WIDTH, HEIGHT, mErrorMessage);
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
