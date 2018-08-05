using Arci.Networking.Data;
using Arci.Networking.Security;
using Arci.Networking.Security.AesOptions;
using Client.Network;
using Client.Enums;
using Client.Security;
using Client.Data;
using System;
using System.Linq;
using System.Security.Cryptography;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using ClientNetwork = Arci.Networking.Client;

namespace Client.Game
{
    public class ClientGame : IDisposable
    {
        private readonly Task networkConnectionTask;
        private readonly CancellationTokenSource tokenSource = new CancellationTokenSource();
        private readonly ClientNetwork network;
        private const int port = 10751;
        private readonly AesEncryptor aes;

        public static string[] Servers { get; } =
        {
            "localhost",
            "calista.mine.sk"
        };

        public MainWindow MainWindow { get; }

        public ChatHandler Chat { get; }

        public Player Player { get; set; }

        public Player Opponent { get; set; }

        private ClientGame(string name, MainWindow window, ClientNetwork network)
        {
            MainWindow = window;
            this.network = network;
            Chat = new ChatHandler(this);
            Player = new Player(this, MainWindow.PlayerCard1, MainWindow.PlayerCard2, MainWindow.PlayerCard3, MainWindow.PlayerCard4) { Name = name };
            Opponent = new Player(this, MainWindow.OpponentCard1, MainWindow.OpponentCard2, MainWindow.OpponentCard3, MainWindow.OpponentCard4);

            // Sends init packet to server
            var packet = new Packet(CMSGPackets.CMSG_INIT_PACKET);
            var rsa = new RsaEncryptor(RSAKey.Modulus, RSAKey.Exponent);
            aes = new AesEncryptor(AesEncryptionType.Aes256Bits) { PaddingMode = PaddingMode.PKCS7 };
            network.Encryptor = aes;
            packet.Write(rsa.Encrypt(aes.Encryptors));
            packet.Write(aes.Encrypt(name));
            rsa.Dispose();

            SendPacket(packet, false);
            networkConnectionTask = Task.Run(UpdateAsync, tokenSource.Token);
        }

        // Creates new instance of game
        public static async Task<ClientGame> CreateAsync(string name, string server, MainWindow window)
        {
            var network = await ClientNetwork.CreateAsync(server, port);
            if (network == null)
                return null;

            return new ClientGame(name, window, network);
        }

        // Gets player by id
        public Player GetPlayer(UInt32 playerId)
        {
            var player = Player ?? Opponent;
            if (player == null)
                return null;

            return Player.Id == playerId ? Player : Opponent;
        }

        // Gets opponent to player by id
        public Player GetOpponent(UInt32 playerId)
        {
            var player = Player ?? Opponent;
            if (player == null)
                return null;

            return Player.Id == playerId ? Opponent : Player;
        }

        // Determines if the player is opponent
        public bool IsOpponent(UInt32 playerId)
        {
            return Opponent?.Id == playerId;
        }

        // Sends packet to server
        public void SendPacket(Packet packet, bool encrypt = true, bool disposePacket = true) 
        { 
            network?.SendPacket(packet, encrypt);
            if (disposePacket)
                packet.Dispose();
        }

        // Removes all resources
        public void Dispose()
        {
            if (network != null)
            {
                // Cancel task and try to wait
                tokenSource.Cancel();
                networkConnectionTask.Wait(250);

                aes.Dispose();
                network.Dispose();
            }
        }

        // Sends chat message packet to server
        public void SendChatMessage(string text, ChatTypes chatType, params string[] customParams)
        {
            if (chatType == ChatTypes.AutoSelect)
                chatType = Chat.ActiveChat;

            Chat.SendChatMessage(text, chatType, customParams);
        }

        // Handles command written in chat
        public void HandleCommand(string command)
        {
            CommandHandler.HandleCommand(command, this);
        }

        // Invokes action to UI thread
        public void Invoke(Action action)
        {
            MainWindow.Invoke(action);
        }

        // Sends selected cards to server
        public void SendSelectedCards()
        {
            var packet = new Packet(CMSGPackets.CMSG_SELECTED_CARDS);
            var cards = DataHolder.Cards.Where(x => x.SelectionType == SelectionType.Selected);

            packet.Write((byte)cards.Count());
            foreach (var card in cards)
            {
                packet.Write(card.Id);
                card.UnloadImages();
            }

            SendPacket(packet);
        }

        // Sends card to attack to server
        public void SendCardAction(PlayableCard card)
        {
            if (card.SelectionType == SelectionType.None)
                return;

            SetActiveCardActionGrid(false);
            var packet = new Packet(CMSGPackets.CMSG_CARD_ACTION).Builder()
                .WriteGuidBitStreamInOrder(card.Guid, 4, 3, 2, 7, 1, 6, 0, 5)
                .FlushBits()
                .WriteGuidByteStreamInOrder(card.Guid, 6, 2, 7, 1, 0)
                .Write((byte)card.SelectionType)
                .WriteGuidByteStreamInOrder(card.Guid, 5, 3, 4)
                .Build();

            SendPacket(packet);
        }

        // Sends defend action to server
        public void SendDefendSelf()
        {
            SetActiveCardActionGrid(false);
            var packet = new Packet(CMSGPackets.CMSG_DEFEND_SELF);
            SendPacket(packet);
        }

        // Shows card deck
        public void ShowCardDeck(bool visible)
        {
            var visibility = visible ? Visibility.Visible : Visibility.Hidden;
            Invoke(() =>
            {
                MainWindow.CardDeckGrid.Visibility = visibility;
                MainWindow.CardActionGrid.Visibility = visibility;
            });
        }

        // Activates/deactivates card action grid
        public void SetActiveCardActionGrid(bool active)
        {
            Invoke(() =>
            {
                MainWindow.Cursor = Cursors.Arrow;
                MainWindow.CardActionGrid.IsEnabled = active;
                if (active)
                    MainWindow.UseSpellButton.IsEnabled = Player.CanCastSpell(); ;
            });
        }

        // Unloads data
        public void UnloadData()
        {
            MainWindow.SlideShow.UnloadItems();
            DataHolder.UnloadCards();
        }

        // Ends game
        public void EndGame(bool win)
        {
            ShowCardDeck(false);
            MessageBox.Show(win ? "You have won the match!" : "You have lost the match!");
        }

        // Selects targets on which is spell usable
        public bool SetPossibleSpellTargets()
        {
            var spell = Player.ActiveCard.Spell;
            if (spell == null)
                return false;

            var targets = spell.GetPossibleTargets(Player, Opponent);
            Player.SelectPossibleTargets(targets, SelectionType.SpellUsable);
            Opponent.SelectPossibleTargets(targets, SelectionType.SpellUsable);

            return true;
        }

        public PlayableCard GetCardByImageControlName(string imageControlName)
        {
            var cardPair = Player.GetCardByImageControlName(imageControlName);
            if (cardPair != null)
                return cardPair.First;

            cardPair = Opponent.GetCardByImageControlName(imageControlName);
            return cardPair?.First;
        }

        private async Task UpdateAsync()
        {
            while(!tokenSource.Token.IsCancellationRequested)
            {
                try
                {
                    var packets = await network.ReceiveDataAsync(true, tokenSource.Token);
                    if (packets != null)
                    {
                        foreach (var packet in packets)
                        {
                            var packetHandler = PacketHandler.GetPacketHandler(packet.OpcodeNumber);
                            packetHandler(packet, this);
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
