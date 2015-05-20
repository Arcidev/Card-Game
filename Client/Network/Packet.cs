using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Client.Enums;
using Client.Security;

namespace Client.Network
{
    public partial class Packet : IDisposable
    {
        private BinaryReader readData;
        private BinaryWriter writeData;
        private MemoryStream memoryStream;
        private bool encrypt;

        private int bitPos;
        private byte curBitVal;

        public static readonly int MaxPacketSize = 10000;

        // Inicializes writeable packet
        public Packet(CMSGPackets opcodeNumber)
        {
            encrypt = opcodeNumber != CMSGPackets.CMSG_INIT_PACKET;
            Initialize(opcodeNumber);
        }

        // Inicializes readable packet
        public Packet(byte[] data, int length)
        {
            inicialize();
            readData = new BinaryReader(new MemoryStream(AesEncryptor.Decrypt(data, length)));
        }

        // Inicializes data in packet
        public void Initialize(CMSGPackets opcodeNumber)
        {
            inicialize();

            if (writeData != null)
                writeData.Close();
            if (memoryStream != null)
                memoryStream.Close();

            memoryStream = new MemoryStream();
            writeData = new BinaryWriter(memoryStream);
            writeData.Write((UInt16)opcodeNumber);
        }

        // Writes 1 bit
        public void WriteBit(UInt32 bit)
        {
            --bitPos;
            if (bit != 0)
                curBitVal |= (byte)(1 << bitPos);

            if (bitPos == 0)
            {
                bitPos = 8;
                Write(curBitVal);
                curBitVal = 0;
            }
        }

        // Writes guid byte stream in specified order
        public void WriteGuidByteStreamInOrder(Guid guid, params int[] indexes)
        {
            foreach (var index in indexes)
                if (guid[index] != 0)
                    Write(guid[index]);
        }

        // Writes guid bit stream in specified order
        public void WriteGuidBitStreamInOrder(Guid guid, params int[] indexes)
        {
            foreach (var index in indexes)
                WriteBit(guid[index]);
        }

        // Flushes bits from memory to stream
        public void FlushBits()
        {
            if (bitPos == 8)
                return;

            Write(curBitVal);
            curBitVal = 0;
            bitPos = 8;

        }

        // Reads 1 bit
        public bool ReadBit()
        {
            ++bitPos;
            if (bitPos > 7)
            {
                bitPos = 0;
                curBitVal = ReadByte();
            }

            return ((curBitVal >> (7 - bitPos)) & 1) != 0;
        }

        // Writes string value
        public void Write (string val)
		{
			writeData.Write((UInt16)val.Length);
            writeData.Write(System.Text.Encoding.ASCII.GetBytes(val));
		}

        // Reads Int16
        public Int16 ReadInt16() { return readData.ReadInt16(); }

        // Reads Int32
        public Int32 ReadInt32() { return readData.ReadInt32(); }

        // Reads UInt16
        public UInt16 ReadUInt16() { return readData.ReadUInt16(); }

        // Reads UInt32
        public UInt32 ReadUInt32() { return readData.ReadUInt32(); }

        // Reads byte
        public byte ReadByte() { return readData.ReadByte(); }

        // Reads sbyte
        public sbyte ReadSByte() { return readData.ReadSByte(); }

        // Reads multiple bytes
        public byte[] ReadBytes()
        {
            var length = ReadUInt16();
            return readData.ReadBytes(length);
        }

        // Reads multiple bytes
        public byte[] ReadBytes(int length)
        {
            return readData.ReadBytes(length);
        }

        // Reads string
        public string ReadString()
        {
            return System.Text.Encoding.ASCII.GetString(ReadBytes());
        }

        // Reads guid byte stream in specified order
        public void ReadGuidByteStreamInOrder(Guid guid, params int[] indexes)
        {
            foreach (var index in indexes)
                if (guid[index] != 0)
                    guid[index] = ReadByte();
        }

        // Reads guid bit stream in specified order
        public void ReadGuidBitStreamInOrder(Guid guid, params int[] indexes)
        {
            foreach (var index in indexes)
                guid[index] = (byte) (ReadBit() ? 1 : 0);
        }

        // Gets encrypted data
        public byte[] EnryptedData 
        {
            get { return encrypt ? AesEncryptor.Encrypt(memoryStream.ToArray()) : memoryStream.ToArray(); }
        }

        // Disposing object
        public void Dispose()
        {
            if (writeData != null)
                writeData.Close();
            if (memoryStream != null)
                memoryStream.Close();
        }

        private void inicialize()
        {
            bitPos = 8;
            curBitVal = 0;
        }
    }
}
