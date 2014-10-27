package debugclient;

import java.awt.Dimension;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;

import debugclient.comm.FpsMessage;

public class FpsPanel extends JPanel
{
	private JLabel procFps;
	private JLabel ctrlFps;

	public FpsPanel()
	{
		procFps = new JLabel("Processing: 0");
		ctrlFps = new JLabel("Controller: 0");

		add(procFps);
		add(ctrlFps);

		setBorder(BorderFactory.createTitledBorder("FPS"));
		setLayout(new GridLayout(1, 0));
		setMaximumSize(new Dimension(10000, 50));
	}

	public void onFps(FpsMessage msg)
	{
		switch (msg.type)
		{
			case FpsMessage.TYPE_PROC:
				procFps.setText(String.format("Processing: %02d", msg.fps));
				break;

			case FpsMessage.TYPE_CTRL:
				ctrlFps.setText(String.format("Controller: %02d", msg.fps));
				break;
		}
	}
}
