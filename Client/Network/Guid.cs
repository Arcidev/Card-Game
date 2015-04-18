using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Network
{
    public class Guid
    {
        private byte[] byteVal;

        public Guid()
        {
            byteVal = new byte[8];
        }

        public Guid(UInt64 guid)
        {
            byteVal = BitConverter.GetBytes(guid);
        }

        public byte this[int index]
        {
            get { return byteVal[index]; }
            set { byteVal[index] = value; }
        }

        public static implicit operator UInt64(Guid guid)
        {
            return BitConverter.ToUInt64(guid.byteVal, 0);
        }
    }
}
