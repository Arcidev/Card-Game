using Client.Data;
using Client.Enums;
using Client.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Client.Network
{
    public static class PacketHandler
    {
        private static readonly Dictionary<SMSGPackets, Action<Packet, ClientGame>> packetHandlers = new Dictionary<SMSGPackets, Action<Packet, ClientGame>>()
        {
            { SMSGPackets.SMSG_INIT_RESPONSE,                           HandleInitResponse          },
            { SMSGPackets.SMSG_AVAILABLE_CARDS,                         HandleAvailableCards        },
            { SMSGPackets.SMSG_CHAT_MESSAGE,                            HandleChatMessage           },
            { SMSGPackets.SMSG_WHISPER_FAILED,                          HandleWhisperFailed         },
            { SMSGPackets.SMSG_SELECT_CARDS_FAILED,                     HandleSelectCardsFailed     },
            { SMSGPackets.SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER,    HandleSelectCardsWait       },
            { SMSGPackets.SMSG_SELECT_CARDS,                            HandleSelectCards           },
            { SMSGPackets.SMSG_DECK_CARDS,                              HandleDeckCards             },
            { SMSGPackets.SMSG_ACTIVE_PLAYER,                           HandleActivePlayer          },
            { SMSGPackets.SMSG_PLAYER_DISCONNECTED,                     HandlePlayerDisconnected    },
            { SMSGPackets.SMSG_INVALID_TARGET,                          HandleInvalidTarget         }
        };

        // Returns function to handle packet
        public static Action<Packet, ClientGame> GetPacketHandler(SMSGPackets packetType)
        {
            return packetHandlers[packetType];
        }

        // Handle SMSG_INIT_RESPONSE packet
        private static void HandleInitResponse(Packet packet, ClientGame game)
        {
            bool hasOpponent = packet.ReadBit();
            UInt32 playerId = packet.ReadUInt32();
            game.Player.Id = playerId;

            string message;
            if (hasOpponent)
            {
                UInt32 opponentId = packet.ReadUInt32();
                string opponentName = packet.ReadString();
                message = string.Format("{0} has joined the game", opponentName);

                game.Opponent.Id = opponentId;
                game.Opponent.Name = opponentName;
            }
            else
                message = "Waiting for another player to join the game";

            game.Chat.Write(message, ChatTypes.Info);
        }

        // Handle SMSG_AVAILABLE_CARDS packet
        private static void HandleAvailableCards(Packet packet, ClientGame game)
        {
            UInt16 cardsCount = packet.ReadUInt16();

            List<SelectableCard> cards = new List<SelectableCard>();
            for (UInt16 i = 0; i < cardsCount; i++)
                cards.Add(new SelectableCard(packet.ReadUInt32(), (CreatureTypes)packet.ReadByte(), packet.ReadByte(), packet.ReadByte(), packet.ReadByte(), packet.ReadByte()));

            DataHolder.LoadData(cards);
            game.MainWindow.SlideShow.LoadItems();
            game.MainWindow.SlideShow.SetVisible(true);
        }

        // Handle SMSG_CHAT_MESSAGE packet
        private static void HandleChatMessage(Packet packet, ClientGame game)
        {
            ChatTypes chatType = (ChatTypes)packet.ReadByte();
            string senderName = packet.ReadString();
            string message = packet.ReadString();

            game.Chat.WriteChannelMessage(chatType, message, senderName);
        }

        // Handle SMSG_WHISPER_FAILED packet
        private static void HandleWhisperFailed(Packet packet, ClientGame game)
        {
            game.Chat.Write(string.Format("Player \"{0}\" not found", packet.ReadString()), ChatTypes.Info);
        }

        // Handle SMSG_SELECT_CARDS_FAILED packet
        private static void HandleSelectCardsFailed(Packet packet, ClientGame game)
        {
            game.Chat.Write(string.Format("Selecting cards failed: {0}", ((SelectCardFailReason)packet.ReadByte()).GetDescription()), ChatTypes.Info);
        }

        // Handle SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER packet
        private static void HandleSelectCardsWait(Packet packet, ClientGame game)
        {
            game.MainWindow.SlideShow.SetVisible(false);
            game.ShowCardDeck();
            game.Chat.Write("Waiting for another player to pick his cards", ChatTypes.Info);
        }

        // Handle SMSG_SELECT_CARDS packet
        private static void HandleSelectCards(Packet packet, ClientGame game)
        {
            game.MainWindow.SlideShow.SetVisible(false);
            game.ShowCardDeck();

            var count1 = packet.ReadByte();
            var count2 = packet.ReadByte();

            Guid[] guids1 = new Guid[count1];
            Guid[] guids2 = new Guid[count2];

            for (var i = 0; i < count2; i++)
            {
                guids2[i] = new Guid();
                packet.ReadGuidBitStreamInOrder(guids2[i], 1, 2, 7, 0, 5, 3, 4, 6);
            }

            for (var i = 0; i < count1; i++)
            {
                guids1[i] = new Guid();
                packet.ReadGuidBitStreamInOrder(guids1[i], 7, 1, 2, 4, 6, 0, 3, 5);
            }

            var senderId = packet.ReadUInt32();
            Player player1 = (game.Player.Id == senderId) ? game.Player : game.Opponent;
            Player player2 = (game.Player.Id == senderId) ? game.Opponent : game.Player;

            List<PlayableCard> cards1 = new List<PlayableCard>();
            List<PlayableCard> cards2 = new List<PlayableCard>();

            for (var i = 0; i < count1; i++)
            {
                packet.ReadGuidByteStreamInOrder(guids1[i], 7, 2, 0);

                byte damage = packet.ReadByte();
                byte health = packet.ReadByte();

                packet.ReadGuidByteStreamInOrder(guids1[i], 1, 6, 4, 5);

                UInt32 id = packet.ReadUInt32();
                byte defense = packet.ReadByte();
                CreatureTypes type = (CreatureTypes)packet.ReadByte();

                packet.ReadGuidByteStreamInOrder(guids1[i], 3);

                byte mana = packet.ReadByte();
                PlayableCard card = PlayableCard.Create(guids1[i], id, type, health, damage, mana, defense);
                cards1.Add(card);
            }

            for (var i = 0; i < count2; i++)
            {
                CreatureTypes type = (CreatureTypes)packet.ReadByte();
                byte defense = packet.ReadByte();
                byte damage = packet.ReadByte();

                packet.ReadGuidByteStreamInOrder(guids2[i], 4, 2, 6, 1, 7, 0);

                byte mana = packet.ReadByte();
                byte health = packet.ReadByte();
                UInt32 id = packet.ReadUInt32();

                packet.ReadGuidByteStreamInOrder(guids2[i], 3, 5);

                PlayableCard card = PlayableCard.Create(guids2[i], id, type, health, damage, mana, defense);
                cards2.Add(card);
            }

            player1.AddCards(cards1);
            player2.AddCards(cards2);

            game.UnloadData();
            game.Chat.Write("Game has started", ChatTypes.Info);
        }

        // Handle SMSG_DECK_CARDS packet
        private static void HandleDeckCards(Packet packet, ClientGame game)
        {
            var cardsCount = packet.ReadByte();
            Guid[] guids = new Guid[cardsCount];
            for (byte i = 0; i < cardsCount; i++)
            {
                guids[i] = new Guid();
                packet.ReadGuidBitStreamInOrder(guids[i], 7, 2, 1, 4, 5, 0, 6, 3);
            }

            var player = game.GetPlayer(packet.ReadUInt32());
            if (player == null)
                return;

            for (byte i = 0; i < cardsCount; i++)
            {
                packet.ReadGuidByteStreamInOrder(guids[i], 2, 1, 7, 6, 0, 5, 3, 4);
                player.PutCardOnDeck(guids[i], i);
            }
        }

        // Handle SMSG_ACTIVE_PLAYER packet
        private static void HandleActivePlayer(Packet packet, ClientGame game)
        {
            Guid cardGuid = new Guid();
            packet.ReadGuidBitStreamInOrder(cardGuid, 7, 1, 2, 0, 3, 5, 4, 6);

            packet.ReadGuidByteStreamInOrder(cardGuid, 7, 5, 4, 2, 6);
            var activePlayerId = packet.ReadUInt32();
            Player activePlayer = (game.Player.Id == activePlayerId) ? game.Player : game.Opponent;
            Player nonActivePlayer = (game.Player.Id == activePlayerId) ? game.Opponent : game.Player;

            packet.ReadGuidByteStreamInOrder(cardGuid, 1, 0, 3);
            nonActivePlayer.SetWaitingState();
            activePlayer.SetActiveState(cardGuid);
            game.SetActiveCardActionGrid(game.Player.Id == activePlayerId);
        }

        // Handle SMSG_PLAYER_DISCONNECTED packet
        private static void HandlePlayerDisconnected(Packet packet, ClientGame game)
        {
            game.Chat.Write(string.Format("Player \"{0}\" has disconnected", game.Opponent.Name), ChatTypes.Info);
        }

        // Handle SMSG_INVALID_TARGET packet
        private static void HandleInvalidTarget(Packet packet, ClientGame game)
        {
            game.Chat.Write("You cannot attack that target", ChatTypes.Info);
        }
    }
}
