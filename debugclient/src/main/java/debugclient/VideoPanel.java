package debugclient;

import java.awt.Graphics;
import java.awt.Image;

import javax.swing.JPanel;

public class VideoPanel extends JPanel
{
	private Image mFrame;

	public VideoPanel()
	{
	}

	public void setFrame(Image frame)
	{
		mFrame = frame;
		repaint();
	}

	@Override
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		if (mFrame == null)
		{
			return;
		}

		int width = mFrame.getWidth(null);
		int height = mFrame.getHeight(null);

		g.drawImage(mFrame, 0, 0, width, height, 0, 0, width, height, null);
	}
}
