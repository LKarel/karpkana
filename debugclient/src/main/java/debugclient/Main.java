package debugclient;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import com.google.protobuf.Message;

import debugclient.comm.Connection;

public class Main
{
	public static void main(String[] args)
	{
		javax.swing.SwingUtilities.invokeLater(new Runnable()
		{
			public void run()
			{
				showGUI();
			}
		});
	}

	private static void showGUI()
	{
		JFrame frame = new JFrame("DebugClient");
		frame.setSize(800, 600);
		frame.setLocationRelativeTo(null);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(new BoxLayout(frame.getContentPane(), BoxLayout.Y_AXIS));

		final FpsPanel fpsPanel = new FpsPanel();
		final VideoPanel videoPanel = new VideoPanel();
		final VideoPanel classifyPanel = new VideoPanel();

		final Connection connection = new Connection()
		{
			protected void onError(Throwable e)
			{
				System.out.println(e);
			}

			protected void onMessage(Message msg)
			{
				String name = msg.getDescriptorForType().getFullName();

				if (name.equals("c22dlink.FrameImage"))
				{
					c22dlink.FrameImage frameImage = (c22dlink.FrameImage) msg;

					if (frameImage.getType() == c22dlink.FrameImage.Type.ORIGINAL)
					{
						videoPanel.putFrame(frameImage);
					}
					else
					{
						classifyPanel.putFrame(frameImage);
					}
				}
			}
		};

		ConnectPanel connectPanel = new ConnectPanel()
		{
			public void onConnect(String hostname, int port)
			{
				connection.connect(hostname, port);
			};
		};

		JPanel topPanel = new JPanel();
		topPanel.setLayout(new GridLayout(1, 0));
		topPanel.add(connectPanel);
		topPanel.add(fpsPanel);

		JPanel videosPanel = new JPanel();
		videosPanel.setLayout(new GridLayout(1, 0));
		videosPanel.add(classifyPanel);
		videosPanel.add(videoPanel);

		frame.add(topPanel);
		frame.add(videosPanel);

		frame.pack();
		frame.setVisible(true);
	}
}
