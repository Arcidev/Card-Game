using Arci.Networking.Data;
using Arci.Networking.Security;
using Arci.Networking.Security.AesOptions;
using Client.Logic.Data;
using Client.Logic.Data.Achievements;
using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using Client.Logic.Network;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
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
        private readonly CancellationTokenSource tokenSource = new();
        private readonly Dictionary<UInt32, Achievement> achievements = [];

        private List<(string name, bool isOnline)> friends;
        private List<string> friendRequests;
        private List<string> blockedUsers;

        public event Action<MessageType, string> MessageReceived;
        public event Action<UInt16> PacketProcessed;
        public event Action<bool> GameEnded;
        public event Action<string> PropertyChanged;

        /// <summary>
        /// List of available servers
        /// </summary>
        public static string[] Servers { get; } =
        [
            "localhost"
        ];

        public Player Player { get; set; }

        public Player Opponent { get; set; }

        public bool IsGameWaiting { get; set; }

        public Chat Chat { get; }

        public CombatLog CombatLog { get; }

        public IDataHolder DataHolder { get; }

        public IEnumerable<Achievement> Achievements => achievements.Values;

        public IEnumerable<(string name, bool isOnline)> Friends
        {
            get => friends;
            set
            {
                friends = value?.ToList();
                OnPropertyChanged();
            }
        }

        public IEnumerable<string> FriendRequets
        {
            get => friendRequests;
            set
            {
                friendRequests = value?.ToList();
                OnPropertyChanged();
            }
        }

        public IEnumerable<string> BlockedUsers
        {
            get => blockedUsers;
            set
            {
                blockedUsers = value?.ToList();
                OnPropertyChanged();
            }
        }

        private Game(ClientNetwork clientNetwork, IDataHolder dataHolder)
        {
            network = clientNetwork;
            DataHolder = dataHolder;
            Chat = new Chat(this);
            CombatLog = new CombatLog();

            aes = new AesEncryptor(AesEncryptionType.Aes256Bits) { PaddingMode = PaddingMode.PKCS7 };
            network.Encryptor = aes;
            networkConnectionTask = Task.Run(UpdateAsync, tokenSource.Token);
        }

        /// <summary>
        /// Creates new instance of game
        /// </summary>
        /// <param name="server">Server url</param>
        /// <param name="dataHolder">Data Holder</param>
        /// <param name="packetCallback">Callback for processed packets</param>
        /// <returns>Instance of game</returns>
        public static async Task<Game> CreateAsync(string server, IDataHolder dataHolder, Action<UInt16> packetCallback)
        {
            var network = await ClientNetwork.CreateAsync(server, port);
            if (network == null)
                return null;

            var game = new Game(network, dataHolder);
            game.PacketProcessed += packetCallback;

            var packet = new Packet(CMSGPackets.Init);
            using (var rsa = new RsaEncryptor(Security.RSAKey.Modulus, Security.RSAKey.Exponent))
            {
                packet.Write(rsa.Encrypt(game.aes.Encryptors));
            }

            await game.SendPacketAsync(packet, false);
            return game;
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

        public async Task DefendSelfAsync()
        {
            var packet = new Packet(CMSGPackets.DefendSelf);
            await SendPacketAsync(packet);
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
            if (MessageReceived != null)
            {
                foreach (var handler in MessageReceived.GetInvocationList())
                    MessageReceived -= handler as Action<MessageType, string>;
            }

            if (PacketProcessed != null)
            {
                foreach (var handler in PacketProcessed.GetInvocationList())
                    PacketProcessed -= handler as Action<UInt16>;
            }
        }

        internal void OnGameEnded(bool hasWon)
        {
            IsGameWaiting = false;
            GameEnded?.Invoke(hasWon);
        }

        internal void OnErrorOccured(string error) => MessageReceived?.Invoke(MessageType.Error, error);

        internal void OnInformationReceived(string msg) => MessageReceived?.Invoke(MessageType.Information, msg);

        internal void OnAchievementEarned(string achievement) => MessageReceived?.Invoke(MessageType.Achievement, achievement);

        internal Player GetPlayer(UInt32 playerId)
        {
            if (Player?.Id == playerId)
                return Player;

            if (Opponent?.Id == playerId)
                return Opponent;

            Debug.Assert(true, $"Player with Id:{playerId} not found");
            return null;
        }

        internal Player GetOpponent(UInt32 playerId)
        {
            if (Player?.Id == playerId)
                return Opponent;

            if (Opponent?.Id == playerId)
                return Player;

            Debug.Assert(true, $"Player with Id:{playerId} not found");
            return null;
        }

        internal PlayableCard GetCard(UInt64 cardGuid) => Player?.GetCard(cardGuid) ?? Opponent?.GetCard(cardGuid);

        internal void SwapCards(UInt64 cardGuid1, UInt64 cardGuid2)
        {
            var card1 = GetCard(cardGuid1);
            var card2 = GetCard(cardGuid2);

            var player1 = card1.Player;
            var player2 = card2.Player;

            var idx1 = player1.GetCardIndex(card1);
            var idx2 = player2.GetCardIndex(card2);

            player1.SetCard(card2, idx1);
            player2.SetCard(card1, idx2);
            CombatLog.LoagSwapCard(card1.Name, card2.Name);
        }

        internal void SetAchievements(List<Achievement> achievementsList)
        {
            achievements.Clear();
            foreach (var achievement in achievementsList.OrderBy(x => x.Id))
            {
                if (achievement.ParentId == 0)
                {
                    achievements.Add(achievement.Id, achievement);
                    continue;
                }

                if (achievements.TryGetValue(achievement.ParentId, out var value) && value.IsCompleted)
                {
                    if (value.ParentId > 0)
                        achievements.Remove(value.ParentId);

                    achievements.Add(achievement.Id, achievement);
                }
            }
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

        private void OnPropertyChanged([CallerMemberName] string propertyName = null) => PropertyChanged?.Invoke(propertyName);
    }
}
