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

import debugclient.comm.Connection;
import debugclient.comm.FpsMessage;
import debugclient.comm.FrameMessage;
import debugclient.comm.BlobMessage;
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
		final VideoPanel videoPanel = new VideoPanel();

		final Connection connection = new Connection()
		{
			protected void onError(Throwable e)
			{
				videoPanel.setErrorMessage("CONNECTION LOST");
			}

			protected void onMessage(Message msg)
			{
				if (msg instanceof FrameMessage)
				{
					videoPanel.setFrame((FrameMessage) msg);
				}
				else if (msg instanceof BlobMessage)
				{
					videoPanel.putBlob((BlobMessage) msg);
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
		topPanel.setLayout(new GridLayout(1, 0));
		topPanel.add(connectPanel);
		topPanel.add(fpsPanel);

		frame.add(topPanel);
		frame.add(videoPanel);

		frame.pack();
		frame.setVisible(true);
	}
}
