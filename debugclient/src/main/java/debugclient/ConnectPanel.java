package debugclient;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;

public abstract class ConnectPanel extends JPanel
{
	public ConnectPanel()
	{
		final JTextField connectField = new JTextField();
		connectField.setText("127.0.0.1:11000");
		connectField.setColumns(20);

		JButton connect = new JButton("Connect");
		connect.setEnabled(true);
		connect.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent event)
			{
				String[] split = connectField.getText().split(":");
				int port = 11000;

				if (split.length == 2)
				{
					port = Integer.parseInt(split[1]);
				}

				onConnect(split[0], port);
			}
		});

		add(connectField);
		add(connect);

		setBorder(BorderFactory.createTitledBorder("Connection"));
		setMaximumSize(new Dimension(10000, 50));
	}

	public abstract void onConnect(String hostname, int port);
}
