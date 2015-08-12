


using System;
namespace Client.Network
{
	public partial class Packet : IDisposable
	{
					public void Write (Int16 val)
			{
				writeData.Write(val);
			}
					public void Write (Int32 val)
			{
				writeData.Write(val);
			}
					public void Write (UInt16 val)
			{
				writeData.Write(val);
			}
					public void Write (UInt32 val)
			{
				writeData.Write(val);
			}
					public void Write (Byte val)
			{
				writeData.Write(val);
			}
		
					public void Write (Byte[] val)
			{
				writeData.Write((UInt16)val.Length);
				writeData.Write(val);
			}
			}
}