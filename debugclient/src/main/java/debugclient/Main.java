package debugclient;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import debugclient.comm.Connection;
import debugclient.comm.FpsMessage;
import debugclient.comm.FrameMessage;
import debugclient.comm.BallMessage;
import debugclient.comm.MessageMessage;
import debugclient.comm.Message;

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
		final OutputPanel outputPanel = new OutputPanel();
		final VideoPanel videoPanel = new VideoPanel();

		final Connection connection = new Connection()
		{
			protected void onError(Throwable e)
			{
				outputPanel.putMessage("Connection error: " + e.toString());
				videoPanel.setErrorMessage("CONNECTION LOST");
			}

			protected void onMessage(Message msg)
			{
				if (msg instanceof FrameMessage)
				{
					videoPanel.setFrame((FrameMessage) msg);
				}
				else if (msg instanceof BallMessage)
				{
					videoPanel.putBall((BallMessage) msg);
				}
				else if (msg instanceof MessageMessage)
				{
					outputPanel.putMessage("MSG: " + msg);
				}
				else if (msg instanceof FpsMessage)
				{
					fpsPanel.onFps((FpsMessage) msg);
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
		topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.X_AXIS));
		topPanel.add(connectPanel);
		topPanel.add(fpsPanel);

		frame.add(topPanel);
		frame.add(videoPanel);
		frame.add(outputPanel);

		frame.pack();
		frame.setVisible(true);
	}
}
