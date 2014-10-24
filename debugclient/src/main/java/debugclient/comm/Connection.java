package debugclient.comm;

import java.io.IOException;
import java.net.Socket;

public abstract class Connection
{
	private Socket mSocket;

	public Connection()
	{
	}

	/**
	 * Connect to a server.
	 */
	public void connect(String hostname, int port)
	{
		if (this.mSocket != null)
		{
			try
			{
				this.mSocket.close();
			}
			catch (IOException e)
			{
			}
		}

		try
		{
			this.mSocket = new Socket(hostname, port);

			new Thread(new StreamParser(this.mSocket.getInputStream())
			{
				protected void onError(Throwable e)
				{
					Connection.this.onError(e);
				}

				protected void onMessage(Message msg)
				{
					Connection.this.onMessage(msg);
				}
			}).start();
		}
		catch (IOException e)
		{
			onError(e);
		}
	}

	protected abstract void onError(Throwable e);
	protected abstract void onMessage(Message msg);
}
