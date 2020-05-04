﻿using Arci.Networking.Data;
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

        public Player Player { get; set; }

        public Player Opponent { get; set; }

        public Chat Chat { get; }

        private Game(ClientNetwork clientNetwork)
        {
            network = clientNetwork;
            Chat = new Chat(this);

            var packet = new Packet(CMSGPackets.Init);
            using (var rsa = new RsaEncryptor(Security.RSAKey.Modulus, Security.RSAKey.Exponent))
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
        public void SendPacket(Packet packet, bool encrypt = true, bool disposePacket = true)
        {
            network.SendPacket(packet, encrypt);

            if (disposePacket)
                packet.Dispose();
        }

        /// <summary>
        /// Sends packet to server asynchronously
        /// </summary>
        /// <param name="packet"></param>
        /// <param name="encrypt"></param>
        /// <param name="disposePacket"></param>
        public async Task SendPacketAsync(Packet packet, bool encrypt = true, bool disposePacket = true)
        {
            await network.SendPacketAsync(packet, encrypt);

            if (disposePacket)
                packet.Dispose();
        }

        /// <inheritdoc />
        public void Dispose()
        {
            tokenSource.Cancel();
            networkConnectionTask.Wait(250);

            aes.Dispose();
            network.Dispose();
        }

        public void UnsubscribeAllHandlers()
        {
            if (ErrorOccured != null)
            {
                foreach (var handler in ErrorOccured.GetInvocationList())
                    ErrorOccured -= handler as Action<string>;
            }

            if (PacketProcessed != null)
            {
                foreach (var handler in PacketProcessed.GetInvocationList())
                    PacketProcessed -= handler as Action<UInt16>;
            }
        }

        internal void OnErrorOccured(string error)
        {
            ErrorOccured?.Invoke(error);
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
