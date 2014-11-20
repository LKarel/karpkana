package debugclient.comm;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

import javax.imageio.ImageIO;

import com.google.protobuf.Message;

public abstract class StreamParser implements Runnable
{
	private DataInputStream mIn;

	public StreamParser(InputStream stream)
	{
		mIn = new DataInputStream(stream);
	}

	public void run()
	{
		while (true)
		{
			try
			{
				onMessage(readNextMessage());
			}
			catch (EOFException e)
			{
				onError(e);
				break;
			}
			catch (IOException e)
			{
				onError(e);
				break;
			}
		}
	}

	private Message readNextMessage()
		throws EOFException, IOException
	{
		int type = mIn.read();
		int len = mIn.readInt();

		if (type < 0 || len < 0)
		{
			throw new EOFException();
		}

		byte[] buf = new byte[len];
		mIn.readFully(buf);

		switch (type)
		{
			case 0x10: return (Message) c22dlink.FrameImage.parseFrom(buf);
		}

		return null;
	}

	protected abstract void onError(Throwable e);
	protected abstract void onMessage(Message msg);
}
