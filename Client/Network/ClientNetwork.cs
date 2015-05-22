using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace Client.Network
{
    public class ClientNetwork : IDisposable
    {
        private TcpClient tcpClnt;
        private NetworkStream stream;
        private static readonly int port = 10751;
        private static readonly string[] servers =
        {
            "localhost",
            "calista.mine.sk"
        };

        // Returns available servers list
        public static string[] Servers { get { return servers; } }

        private ClientNetwork(TcpClient client)
        {
            tcpClnt = client;
            stream = tcpClnt.GetStream();
        }

        // Creates new network instance
        public static ClientNetwork Create(string server)
        {
            TcpClient client = null;

            // check connection
            try
            {
                client = new TcpClient(server, port);
            }
            catch(SocketException)
            {
                return null;
            }

            return new ClientNetwork(client);
        }

        // Sends packet to server
        public void SendPacket(Packet packet)
        {
            var encryptedData = packet.EnryptedData;
            var toSend = BitConverter.GetBytes((UInt16)encryptedData.Length).Concat(encryptedData).ToArray();

            stream.Write(toSend, 0, encryptedData.Length + sizeof(UInt16));
            packet.Dispose();
        }

        // Receive data from server
        public byte[] ReceiveData()
        {
            if (stream.DataAvailable)
            {
                var data = new byte[Packet.MaxPacketSize];
                int length = stream.Read(data, 0, Packet.MaxPacketSize);
                return data.Take(length).ToArray();
            }

            return null;
        }

        // Free all resources
        public void Dispose()
        {
            stream.Close();
            tcpClnt.Close();
        }
    }
}
