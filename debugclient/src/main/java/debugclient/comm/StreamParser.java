package debugclient.comm;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

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
			case Protocol.TYPE_MSG: return readNextMsg();
			case Protocol.TYPE_FRAME: return readNextFrame();
		}

		return null;
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

		byte[] buf = new byte[width * height];
		mIn.readFully(buf);

		Image image = bytesToImage(buf, width, height);

		return new FrameMessage(sequence, height / frame_height, image);
	}

	private Image bytesToImage(byte[] bytes, int width, int height)
	{
		int[] pixels = new int[width * height];

		for (int i = 0; i < width * height; i++)
		{
			pixels[i] = bytes[i];
		}

		BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
		WritableRaster raster = image.getRaster();
		raster.setPixels(0, 0, width, height, pixels);

		return image;
	}

	protected abstract void onError(Throwable e);
	protected abstract void onMessage(Message msg);
}
