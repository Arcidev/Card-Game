using Arci.Networking.Data;
using Arci.Networking.Security;
using System;
using System.Linq;
using Client.Network;
using Client.Enums;
using Client.Security;
using Client.Data;
using System.Windows;
using System.Windows.Input;
using ClientNetwork = Arci.Networking.Client;
using System.Threading.Tasks;
using Arci.Networking.Security.AesOptions;
using System.Security.Cryptography;
using System.Threading;

namespace Client.Game
{
    public class ClientGame : IDisposable
    {
        private Task networkConnectionTask;
        private CancellationTokenSource tokenSource = new CancellationTokenSource();
        private ClientNetwork network;
        private MainWindow mainWindow;
        private ChatHandler chat;
        private static readonly string[] servers =
        {
            "localhost",
            "calista.mine.sk"
        };
        private static readonly int port = 10751;

        public static string[] Servers { get { return servers; } }
        public MainWindow MainWindow { get { return mainWindow; } }
        public ChatHandler Chat { get { return chat; } }
        public Player Player { get; set; }
        public Player Opponent { get; set; }

        private ClientGame(string name, MainWindow window, ClientNetwork network)
        {
            mainWindow = window;
            this.network = network;
            chat = new ChatHandler(this);
            Player = new Player(this, MainWindow.PlayerCard1, MainWindow.PlayerCard2, MainWindow.PlayerCard3, MainWindow.PlayerCard4) { Name = name };
            Opponent = new Player(this, MainWindow.OpponentCard1, MainWindow.OpponentCard2, MainWindow.OpponentCard3, MainWindow.OpponentCard4);

            // Sends init packet to server
            Packet packet = new Packet(CMSGPackets.CMSG_INIT_PACKET);
            RsaEncryptor rsa = new RsaEncryptor(RSAKey.Modulus, RSAKey.Exponent);
            AesEncryptor aes = new AesEncryptor(AesEncryptionType.Aes256Bits) { PaddingMode = PaddingMode.PKCS7 };
            network.AesEncryptor = aes;
            packet.Write(rsa.Encrypt(aes.Encryptors));
            packet.Write(aes.Encrypt(name));
            rsa.Dispose();

            SendPacket(packet, false);
            packet.Dispose();

            tokenSource.Token.ThrowIfCancellationRequested();
            networkConnectionTask = Task.Run(UpdateAsync, tokenSource.Token);
        }

        // Creates new instance of game
        public static async Task<ClientGame> CreateAsync(string name, string server, MainWindow window)
        {
            ClientNetwork network = await ClientNetwork.CreateAsync(server, port);
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

            return (Player.Id == playerId) ? Player : Opponent;
        }

        // Gets opponent to player by id
        public Player GetOpponent(UInt32 playerId)
        {
            var player = Player ?? Opponent;
            if (player == null)
                return null;

            return (Player.Id == playerId) ? Opponent : Player;
        }

        public bool IsOpponent(UInt32 playerId)
        {
            return (Opponent != null) && (Opponent.Id == playerId);
        }

        // Sends packet to server
        public void SendPacket(Packet packet, bool encrypt = true) 
        { 
            if (network != null)
                network.SendPacket(packet, encrypt);
        }

        // Removes all resources
        public void Dispose()
        {
            if (network != null)
            {
                // Cancel task and try to wait
                tokenSource.Cancel();
                networkConnectionTask.Wait(250);

                network.AesEncryptor.Dispose();
                network.Dispose();
            }
        }

        // Sends chat message packet to server
        public void SendChatMessage(string text, ChatTypes chatType, params string[] customParams)
        {
            if (chatType == ChatTypes.AutoSelect)
                chatType = chat.ActiveChat;

            chat.SendChatMessage(text, chatType, customParams);
        }

        // Handles command written in chat
        public void HandleCommand(string command)
        {
            CommandHandler.HandleCommand(command, this);
        }

        // Invokes action to UI thread
        public void Invoke(Action action)
        {
            mainWindow.Invoke(action);
        }

        // Sends selected cards to server
        public void SendSelectedCards()
        {
            Packet packet = new Packet(CMSGPackets.CMSG_SELECTED_CARDS);
            var cards = DataHolder.Cards.Where(x => x.SelectionType == SelectionType.Selected);

            packet.Write((byte)cards.Count());
            foreach (var card in cards)
            {
                packet.Write(card.Id);
                card.UnloadImages();
            }

            SendPacket(packet);
            packet.Dispose();
        }

        // Sends card to attack to server
        public void SendCardAction(PlayableCard card)
        {
            if (card.SelectionType == SelectionType.None)
                return;

            SetActiveCardActionGrid(false);
            Packet packet = new Packet(CMSGPackets.CMSG_CARD_ACTION);
            packet.WriteGuidBitStreamInOrder(card.Guid, 4, 3, 2, 7, 1, 6, 0, 5);
            packet.FlushBits();

            packet.WriteGuidByteStreamInOrder(card.Guid, 6, 2, 7, 1, 0);
            packet.Write((byte)card.SelectionType);
            packet.WriteGuidByteStreamInOrder(card.Guid, 5, 3, 4);

            SendPacket(packet);
            packet.Dispose();
        }

        // Sends defend action to server
        public void SendDefendSelf()
        {
            SetActiveCardActionGrid(false);
            Packet packet = new Packet(CMSGPackets.CMSG_DEFEND_SELF);
            SendPacket(packet);
            packet.Dispose();
        }

        // Shows card deck
        public void ShowCardDeck(bool visible)
        {
            Visibility visibility = visible ? Visibility.Visible : Visibility.Hidden;
            Invoke(new Action(delegate()
            {
                MainWindow.CardDeckGrid.Visibility = visibility;
                MainWindow.CardActionGrid.Visibility = visibility;
            }));
        }

        // Activates/deactivates card action grid
        public void SetActiveCardActionGrid(bool active)
        {
            Invoke(new Action(delegate()
            {
                MainWindow.Cursor = Cursors.Arrow;
                MainWindow.CardActionGrid.IsEnabled = active;
                if (active)
                    MainWindow.UseSpellButton.IsEnabled = Player.CanCastSpell(); ;
            }));
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
            if (cardPair != null)
                return cardPair.First;

            return null;
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
