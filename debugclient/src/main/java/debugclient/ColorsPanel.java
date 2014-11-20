package debugclient;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import debugclient.comm.Connection;

public class ColorsPanel extends JPanel
{
	private Connection connection = null;

	public ColorsPanel()
	{
		setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
	}

	public void onColorsInfo(c22dlink.ColorsInfo colors)
	{
		removeAll();

		for (c22dlink.ColorInfo color : colors.getColorsList())
		{
			add(new ColorPanel(color)
			{
				@Override
				public void onChange()
				{
					if (connection != null)
					{
						connection.message(encodeColorInfo());
					}
				}
			});
		}

		revalidate();
		repaint();
	}

	public void onConnection(Connection connection)
	{
		this.connection = connection;
	}

	private abstract class ColorPanel extends JPanel
		implements ActionListener
	{
		private int id;

		private JTextField yLowField;
		private JTextField yHighField;

		private JTextField uLowField;
		private JTextField uHighField;

		private JTextField vLowField;
		private JTextField vHighField;

		private JTextField mergeField;
		private JTextField expectedField;

		public ColorPanel(c22dlink.ColorInfo colorInfo)
		{
			id = colorInfo.getId();

			setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
			setBorder(BorderFactory.createTitledBorder(colorInfo.getName()));

			JPanel yPanel = new JPanel();
			yPanel.add(new JLabel("Y:"));
			yLowField = new JTextField(3);
			yLowField.setText(Integer.toString(colorInfo.getYuvLow().getY()));
			yLowField.addActionListener(this);
			yPanel.add(yLowField);
			yHighField = new JTextField(3);
			yHighField.setText(Integer.toString(colorInfo.getYuvHigh().getY()));
			yHighField.addActionListener(this);
			yPanel.add(yHighField);

			JPanel uPanel = new JPanel();
			uPanel.add(new JLabel("U:"));
			uLowField = new JTextField(3);
			uLowField.setText(Integer.toString(colorInfo.getYuvLow().getU()));
			uLowField.addActionListener(this);
			uPanel.add(uLowField);
			uHighField = new JTextField(3);
			uHighField.setText(Integer.toString(colorInfo.getYuvHigh().getU()));
			uHighField.addActionListener(this);
			uPanel.add(uHighField);

			JPanel vPanel = new JPanel();
			vPanel.add(new JLabel("V:"));
			vLowField = new JTextField(3);
			vLowField.setText(Integer.toString(colorInfo.getYuvLow().getV()));
			vLowField.addActionListener(this);
			vPanel.add(vLowField);
			vHighField = new JTextField(3);
			vHighField.setText(Integer.toString(colorInfo.getYuvHigh().getV()));
			vHighField.addActionListener(this);
			vPanel.add(vHighField);

			JPanel mergePanel = new JPanel();
			mergePanel.add(new JLabel("Merge:"));
			mergeField = new JTextField(8);
			mergeField.setText(Double.toString(colorInfo.getMerge()));
			mergeField.addActionListener(this);
			mergePanel.add(mergeField);

			JPanel expectedPanel = new JPanel();
			expectedPanel.add(new JLabel("Expected:"));
			expectedField = new JTextField(8);
			expectedField.setText(Integer.toString(colorInfo.getExpected()));
			expectedField.addActionListener(this);
			expectedPanel.add(expectedField);

			add(yPanel);
			add(uPanel);
			add(vPanel);
			add(mergePanel);
			add(expectedPanel);
		}

		@Override
		public void actionPerformed(ActionEvent event)
		{
			onChange();
		}

		public c22dlink.ColorInfo encodeColorInfo()
		{
			c22dlink.YUV.Builder yuvLowBuilder = c22dlink.YUV.newBuilder();
			yuvLowBuilder.setY(Integer.parseInt(yLowField.getText()));
			yuvLowBuilder.setU(Integer.parseInt(uLowField.getText()));
			yuvLowBuilder.setV(Integer.parseInt(vLowField.getText()));

			c22dlink.YUV.Builder yuvHighBuilder = c22dlink.YUV.newBuilder();
			yuvHighBuilder.setY(Integer.parseInt(yHighField.getText()));
			yuvHighBuilder.setU(Integer.parseInt(uHighField.getText()));
			yuvHighBuilder.setV(Integer.parseInt(vHighField.getText()));

			c22dlink.ColorInfo.Builder builder = c22dlink.ColorInfo.newBuilder();

			builder.setId(id);
			builder.setExpected(Integer.parseInt(expectedField.getText()));
			builder.setMerge(Double.parseDouble(mergeField.getText()));

			builder.setYuvLow(yuvLowBuilder.build());
			builder.setYuvHigh(yuvHighBuilder.build());

			return builder.build();
		}

		public abstract void onChange();
	}
}
