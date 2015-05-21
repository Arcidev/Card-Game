using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Client.Network;
using Client.Security;
using Client.Enums;
using System.Timers;
using System.Windows.Controls;
using Client.Data;
using System.Windows;
using System.Windows.Input;

namespace Client.Game
{
    public class ClientGame : IDisposable
    {
        private ClientNetwork network;
        private Timer timer;
        private MainWindow mainWindow;
        private ChatHandler chat;

        public MainWindow MainWindow { get { return mainWindow; } }
        public ChatHandler Chat { get { return chat; } }
        public Player Player { get; set; }
        public Player Opponent { get; set; }

        public ClientGame(string name, MainWindow window)
        {
            mainWindow = window;
            network = new ClientNetwork();
            chat = new ChatHandler(this);
            Player = new Player(this, MainWindow.PlayerCard1, MainWindow.PlayerCard2, MainWindow.PlayerCard3, MainWindow.PlayerCard4) { Name = name };
            Opponent = new Player(this, MainWindow.OpponentCard1, MainWindow.OpponentCard2, MainWindow.OpponentCard3, MainWindow.OpponentCard4);

            // Sends init packet to server
            Packet packet = new Packet(CMSGPackets.CMSG_INIT_PACKET);
            RsaEncryptor.Inicialize();
            AesEncryptor.Inicialize();
            packet.Write(RsaEncryptor.Encrypt(AesEncryptor.Key));
            packet.Write(AesEncryptor.Encrypt(name));
            RsaEncryptor.Clear();

            SendPacket(packet);
            
            timer = new System.Timers.Timer(50);
            timer.AutoReset = false;
            timer.Elapsed += Update;
            timer.Start();
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
        public void SendPacket(Packet packet) 
        { 
            if (network != null)
                network.SendPacket(packet);
        }

        // Removes all resources
        public void Dispose()
        {
            if (network != null)
            {
                timer.Stop();
                network.Dispose();
            }

            AesEncryptor.Clear();
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
        }

        // Sends defend action to server
        public void SendDefendSelf()
        {
            SetActiveCardActionGrid(false);
            Packet packet = new Packet(CMSGPackets.CMSG_DEFEND_SELF);
            SendPacket(packet);
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
            DataHolder.UnloadData();
        }

        // Ends game
        public void EndGame(bool win)
        {
            ShowCardDeck(false);
            MessageBox.Show(win ? "You have won... You are amazing" : "You have lost... You sucks");
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

        // Checks every 50ms for new packets form server in separated thread
        private void Update(Object source, ElapsedEventArgs e)
        {
            byte[] networkData = network.ReceiveData();
            if (networkData != null)
            {
                while (networkData.Any())
                {
                    var length = BitConverter.ToUInt16(networkData, 0);
                    networkData = networkData.Skip(sizeof(UInt16)).ToArray();
                    Packet packet = new Packet(networkData, length);
                    var packetHandler = PacketHandler.GetPacketHandler((SMSGPackets)packet.ReadUInt16());
                    packetHandler(packet, this);
                    packet.Dispose();

                    networkData = networkData.Skip(length).ToArray();
                }
            }

            timer.Start();
        }
    }
}
