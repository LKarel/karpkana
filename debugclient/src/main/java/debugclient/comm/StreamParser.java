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
		int first = mIn.read();

		if (first < 0)
		{
			throw new EOFException();
		}

		switch (first & 0x0F)
		{
			case Protocol.TYPE_BALL: return readNextBall();
			case Protocol.TYPE_MSG: return readNextMsg();
			case Protocol.TYPE_FRAME: return readNextFrame();
		}

		return null;
	}

	private Message readNextBall()
		throws EOFException, IOException
	{
		int sequence = mIn.readInt();
		int x = mIn.readUnsignedShort();
		int y = mIn.readUnsignedShort();
		int radius = mIn.readUnsignedShort();

		return new BallMessage(sequence, x, y, radius);
	}

	private Message readNextMsg()
		throws EOFException, IOException
	{
		int level = mIn.readByte();
		int len = mIn.readUnsignedShort();
		byte[] buf = new byte[len];

		mIn.readFully(buf);

		return new MessageMessage(level, new String(buf));
	}

	private Message readNextFrame()
		throws EOFException, IOException
	{
		int sequence = mIn.readInt();
		int frame_height = mIn.readUnsignedShort();
		int width = mIn.readUnsignedShort();
		int height = mIn.readUnsignedShort();
		int len = mIn.readInt();

		byte[] buf = new byte[len];
		mIn.readFully(buf);

		InputStream bodyStream = new ByteArrayInputStream(buf);
		BufferedImage image = ImageIO.read(bodyStream);

		return new FrameMessage(sequence, (float) height / (float) frame_height, image);
	}

	protected abstract void onError(Throwable e);
	protected abstract void onMessage(Message msg);
}
