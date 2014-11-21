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
		final JFrame frame = new JFrame("DebugClient");
		frame.setSize(800, 600);
		frame.setLocationRelativeTo(null);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(new BoxLayout(frame.getContentPane(), BoxLayout.Y_AXIS));

		final VideoPanel videoPanel = new VideoPanel();
		final VideoPanel classifyPanel = new VideoPanel();
		final ColorsPanel colorsPanel = new ColorsPanel()
		{
			public void onPreview(VideoPanel.Channel ch, int low, int high)
			{
				videoPanel.setThreshold(ch, low, high);
			}
		};

		final Connection connection = new Connection()
		{
			protected void onError(Throwable e)
			{
				e.printStackTrace();
				colorsPanel.onConnection(null);
			}

			protected void onMessage(Message msg)
			{
				if (msg == null)
				{
					return;
				}

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
				else if (name.equals("c22dlink.ColorsInfo"))
				{
					colorsPanel.onColorsInfo((c22dlink.ColorsInfo) msg);
					colorsPanel.onConnection(this);
				}
			}
		};

		ConnectPanel connectPanel = new ConnectPanel()
		{
			public void onConnect(String hostname, int port)
			{
				connection.connect(hostname, port);
				connection.message(c22dlink.RequestColors.newBuilder().build());
			};
		};

		JPanel videosPanel = new JPanel();
		videosPanel.setLayout(new GridLayout(1, 0));
		videosPanel.add(classifyPanel);
		videosPanel.add(videoPanel);

		frame.add(connectPanel);
		frame.add(videosPanel);
		frame.add(colorsPanel);

		frame.pack();
		frame.setVisible(true);
	}
}
