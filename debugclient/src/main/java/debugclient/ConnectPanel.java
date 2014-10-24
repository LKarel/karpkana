package debugclient;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

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
				onConnect(split[0], Integer.parseInt(split[1]));
			}
		});

		add(connectField);
		add(connect);
	}

	public abstract void onConnect(String hostname, int port);
}
