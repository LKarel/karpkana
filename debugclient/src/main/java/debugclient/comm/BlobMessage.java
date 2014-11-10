package debugclient.comm;

public class BlobMessage implements Message
{
	public final int sequence;
	public final int color;
	public final int x1;
	public final int x2;
	public final int y1;
	public final int y2;

	public BlobMessage(int sequence, int color, int x1, int x2, int y1, int y2)
	{
		this.sequence = sequence;
		this.color = color;
		this.x1 = x1;
		this.x2 = x2;
		this.y1 = y1;
		this.y2 = y2;
	}

	public int width()
	{
		return Math.abs(this.x2 - this.x1);
	}

	public int height()
	{
		return Math.abs(this.y2 - this.y1);
	}
}
