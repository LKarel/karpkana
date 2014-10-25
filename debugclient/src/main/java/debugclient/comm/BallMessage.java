package debugclient.comm;

public class BallMessage implements Message
{
	public final int sequence;
	public final int x;
	public final int y;
	public final int radius;

	public BallMessage(int sequence, int x, int y, int radius)
	{
		this.sequence = sequence;
		this.x = x;
		this.y = y;
		this.radius = radius;
	}
}
