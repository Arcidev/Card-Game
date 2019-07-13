using Arci.Networking.Data;
using Arci.Networking.Security;
using Arci.Networking.Security.AesOptions;
using Client.Logic.Enums;
using Client.Network;
using System;
using System.Security.Cryptography;
using System.Threading;
using System.Threading.Tasks;
using ClientNetwork = Arci.Networking.Client;

namespace Client.Logic
{
    /// <summary>
    /// Represents the game itself
    /// </summary>
    public class Game : IDisposable
    {
        private const int port = 10751;
        private readonly ClientNetwork network;
        private readonly AesEncryptor aes;
        private readonly Task networkConnectionTask;
        private readonly CancellationTokenSource tokenSource = new CancellationTokenSource();

        public event Action<string> ErrorOccured;
        public event Action<UInt16> PacketProcessed;

        /// <summary>
        /// List of available servers
        /// </summary>
        public static string[] Servers { get; } =
        {
            "localhost",
            "calista.mine.sk"
        };

        private Game(ClientNetwork clientNetwork)
        {
            network = clientNetwork;

            var packet = new Packet(CMSGPackets.Init);
            using (var rsa = new RsaEncryptor(null, null))
            {
                aes = new AesEncryptor(AesEncryptionType.Aes256Bits) { PaddingMode = PaddingMode.PKCS7 };
                network.Encryptor = aes;
                packet.Write(rsa.Encrypt(aes.Encryptors));
            }

            SendPacket(packet, false);
            networkConnectionTask = Task.Run(UpdateAsync, tokenSource.Token);
        }

        /// <summary>
        /// Creates new instance of game
        /// </summary>
        /// <param name="server">Server url</param>
        /// <returns>Instance of game</returns>
        public static async Task<Game> CreateAsync(string server)
        {
            var network = await ClientNetwork.CreateAsync(server, port);
            if (network == null)
                return null;

            return new Game(network);
        }

        /// <summary>
        /// Sends packet to server
        /// </summary>
        /// <param name="packet"></param>
        /// <param name="encrypt"></param>
        /// <param name="disposePacket"></param>
        public void SendPacket(Packet packet, bool encrypt = true)
        {
            network.SendPacket(packet, encrypt);
            packet.Dispose();
        }

        public void OnErrorOccured(string error)
        {
            ErrorOccured?.Invoke(error);
        }

        /// <inheritdoc />
        public void Dispose()
        {
            tokenSource.Cancel();
            networkConnectionTask.Wait(250);

            aes.Dispose();
            network.Dispose();
        }

        private async Task UpdateAsync()
        {
            while (!tokenSource.Token.IsCancellationRequested)
            {
                try
                {
                    var packets = await network.ReceiveDataAsync(true, tokenSource.Token);
                    if (packets != null)
                    {
                        foreach (var packet in packets)
                        {
                            PacketHandler.GetPacketHandler(packet.OpcodeNumber)?.Invoke(packet, this);
                            PacketProcessed?.Invoke(packet.OpcodeNumber);
                            packet.Dispose();
                        }
                    }
                }
                catch (Exception)
                {
                    // Prevent getting stuck
                }
            }
        }
    }
}
