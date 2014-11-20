package debugclient;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
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
	private static final int WIDTH = 640;
	private static final int HEIGHT = 480;

	private BufferedImage mImage = null;

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

	@Override
	public synchronized void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		if (mImage == null)
		{
			return;
		}

		Image image = (Image) Scalr.resize(mImage, Scalr.Method.SPEED,
			Scalr.Mode.FIT_EXACT, WIDTH, HEIGHT, Scalr.OP_ANTIALIAS);

		g.drawImage(image, 0, 0, null);
	}
}
