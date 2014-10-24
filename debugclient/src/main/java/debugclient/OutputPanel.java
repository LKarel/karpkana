package debugclient;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent; import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;

public class OutputPanel extends JPanel
{
	private JTextArea textArea;

	public OutputPanel()
	{
		setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));

		textArea = new JTextArea();
		textArea.setLineWrap(true);
		textArea.setEditable(false);
		textArea.setVisible(true);

		JScrollPane scroll = new JScrollPane(textArea);
		scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);

		add(scroll, BorderLayout.CENTER);
	}

	public void putMessage(String msg)
	{
		this.textArea.append(msg + "\n");
	}
}
