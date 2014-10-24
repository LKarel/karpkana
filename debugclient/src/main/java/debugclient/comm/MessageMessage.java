package debugclient.comm;

public class MessageMessage implements Message
{
	public static final int LEVEL_DEBUG = 1;
	public static final int LEVEL_INFO = 2;
	public static final int LEVEL_WARN = 3;
	public static final int LEVEL_ERROR = 4;

	public final int level;
	public final String msg;

	public MessageMessage(int level, String msg)
	{
		this.level = level;
		this.msg = msg;
	}

	public String toString()
	{
		String levelStr = "DEBUG";

		switch (this.level)
		{
			case LEVEL_INFO: levelStr = "INFO"; break;
			case LEVEL_WARN: levelStr = "WARN"; break;
			case LEVEL_ERROR: levelStr = "ERROR"; break;
		}

		return String.format("%s: %s", levelStr, msg);
	}
}
