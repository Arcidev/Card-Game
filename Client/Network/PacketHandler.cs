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
            { SMSGPackets.SMSG_ATTACK_RESULT,                           HandleAttackResult          },
            { SMSGPackets.SMSG_END_GAME,                                HandleEndGame               },
            { SMSGPackets.SMSG_CARD_STAT_CHANGED,                       HandleCardStatChanged       },
            { SMSGPackets.SMSG_SPELL_CAST_FAILED,                       HandleSpellCastFailed       },
            { SMSGPackets.SMSG_SPELL_DAMAGE,                            HandleSpellDamage           },
            { SMSGPackets.SMSG_APPLY_AURA,                              HandleApplyAura             },
            { SMSGPackets.SMSG_SPELL_PERIODIC_DAMAGE,                   HandleSpellPeriodicDamage   }
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

            Dictionary<UInt32, SelectableCard> cards = new Dictionary<UInt32, SelectableCard>();
            bool[] hasSpell = new bool[cardsCount];
            for (UInt16 i = 0; i < cardsCount; i++)
                hasSpell[i] = packet.ReadBit();

            for (UInt16 i = 0; i < cardsCount; i++)
            {
                UInt32 id = packet.ReadUInt32();
                CreatureTypes type = (CreatureTypes)packet.ReadByte();
                byte health = packet.ReadByte();
                Spell spell = null;
                if (hasSpell[i])
                {
                    byte manaCost = packet.ReadByte();
                    UInt32 spellId = packet.ReadUInt32();
                    byte spellEffectsCount = packet.ReadByte();
                    SpellEffect[] spellEffects = new SpellEffect[spellEffectsCount];
                    for (int j = 0; j < spellEffectsCount; j++)
                        spellEffects[j] = new SpellEffect(packet.ReadByte());

                    spell = new Spell(spellId, manaCost, spellEffects);
                }
                byte damage = packet.ReadByte();
                byte mana = packet.ReadByte();
                byte defense = packet.ReadByte();
                byte price = packet.ReadByte();
                cards.Add(id, new SelectableCard(id, type, health, damage, mana, defense, price, spell));
            }

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
            game.ShowCardDeck(true);
            game.Chat.Write("Waiting for another player to pick his cards", ChatTypes.Info);
        }

        // Handle SMSG_SELECT_CARDS packet
        private static void HandleSelectCards(Packet packet, ClientGame game)
        {
            game.MainWindow.SlideShow.SetVisible(false);
            game.ShowCardDeck(true);

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

            PlayableCard[] cards1 = new PlayableCard[count1];
            PlayableCard[] cards2 = new PlayableCard[count2];

            for (var i = 0; i < count1; i++)
            {
                packet.ReadGuidByteStreamInOrder(guids1[i], 7, 2, 0, 1, 6, 4, 5);
                UInt32 id = packet.ReadUInt32();
                packet.ReadGuidByteStreamInOrder(guids1[i], 3);
                cards1[i] = PlayableCard.Create(guids1[i], DataHolder.GetCard(id));
            }

            for (var i = 0; i < count2; i++)
            {
                packet.ReadGuidByteStreamInOrder(guids2[i], 4, 2, 6, 1, 7, 0);
                UInt32 id = packet.ReadUInt32();
                packet.ReadGuidByteStreamInOrder(guids2[i], 3, 5);
                cards2[i] = PlayableCard.Create(guids2[i], DataHolder.GetCard(id));
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
                packet.ReadGuidByteStreamInOrder(guids[i], 2, 1, 7, 6, 0, 5, 3, 4);

            player.PutCardsOnDeck(Array.ConvertAll(guids, guid => (UInt64)guid));
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

        // Handle SMSG_ATTACK_RESULT packet
        private static void HandleAttackResult(Packet packet, ClientGame game)
        {
            AttackResult result = (AttackResult)packet.ReadByte();
            if (result == AttackResult.InvalidTarget)
            {
                game.SetActiveCardActionGrid(true);
                game.Chat.Write("You cannot attack that target", ChatTypes.Info);
                return;
            }
            else
            {
                Guid cardGuid = new Guid();
                packet.ReadGuidBitStreamInOrder(cardGuid, 6, 2, 1, 7, 3, 0, 4, 5);
                packet.ReadGuidByteStreamInOrder(cardGuid, 2, 6, 7);
                UInt32 attackerId = packet.ReadUInt32();
                packet.ReadGuidByteStreamInOrder(cardGuid, 1, 3, 0);
                byte damage = packet.ReadByte();
                packet.ReadGuidByteStreamInOrder(cardGuid, 5, 4);

                Player opponent = game.GetOpponent(attackerId);
                if (result == AttackResult.CardAttacked)
                    opponent.AttackCard(cardGuid, damage, CombatLogTypes.BasicDamage);
                else
                    opponent.DestroyCard(cardGuid, damage, CombatLogTypes.BasicDamage);
            }
        }

        // Handle SMSG_CARD_STAT_CHANGED packet
        private static void HandleCardStatChanged(Packet packet, ClientGame game)
        {
            Guid guid = new Guid();
            packet.ReadGuidBitStreamInOrder(guid, 2, 6, 7, 1, 0, 3, 5, 4);
            packet.ReadGuidByteStreamInOrder(guid, 5, 7);
            sbyte value = packet.ReadSByte();
            packet.ReadGuidByteStreamInOrder(guid, 6, 3, 1);
            UInt32 playerId = packet.ReadUInt32();
            packet.ReadGuidByteStreamInOrder(guid, 0, 4, 2);
            CardStats cardStat = (CardStats)packet.ReadByte();

            game.GetPlayer(playerId).ModifyCardStat(guid, cardStat, value);
        }

        // Handle SMSG_END_GAME packet
        private static void HandleEndGame(Packet packet, ClientGame game)
        {
            UInt32 winnerId = packet.ReadUInt32();
            game.EndGame(game.Player.Id == winnerId);
        }

        // Handle SMSG_SPELL_CAST_FAILED packet
        private static void HandleSpellCastFailed(Packet packet, ClientGame game)
        {
            SpellFailReason reason = (SpellFailReason)packet.ReadByte();
            game.SetActiveCardActionGrid(true);
            game.Chat.Write(reason.GetDescription(), ChatTypes.Info);
        }

        // Handle SMSG_SPELL_DAMAGE packet
        private static void HandleSpellDamage(Packet packet, ClientGame game)
        {
            byte targetsCount = packet.ReadByte();
            Guid[] targets = new Guid[targetsCount];
            bool[] isAlive = new bool[targetsCount];

            for (int i = 0; i < targetsCount; i++)
            {
                targets[i] = new Guid();
                isAlive[i] = packet.ReadBit();
                packet.ReadGuidBitStreamInOrder(targets[i], 6, 3, 1, 7, 0, 2, 5, 4);
            }

            Player opponent = game.GetOpponent(packet.ReadUInt32());
            for (int i = 0; i < targetsCount; i++)
            {
                packet.ReadGuidByteStreamInOrder(targets[i], 4, 3, 5);
                byte damage = packet.ReadByte();
                packet.ReadGuidByteStreamInOrder(targets[i], 2, 0, 1, 6, 7);

                if (isAlive[i])
                    opponent.AttackCard(targets[i], damage, CombatLogTypes.SpellUsage);
                else
                    opponent.DestroyCard(targets[i], damage, CombatLogTypes.SpellUsage);
            }
        }

        // Handle SMSG_APPLY_AURA packet
        private static void HandleApplyAura(Packet packet, ClientGame game)
        {
            Guid guid = new Guid();
            packet.ReadGuidBitStreamInOrder(guid, 7, 2, 1, 3, 5, 4, 0, 6);

            Player player = game.GetPlayer(packet.ReadUInt32());
            packet.ReadGuidByteStreamInOrder(guid, 0, 5, 2, 1, 7, 6, 4, 3);
            UInt32 spellId = packet.ReadUInt32();

            player.ApplyAura(guid, spellId);
        }

        // Handle SMSG_SPELL_PERIODIC_DAMAGE packet
        private static void HandleSpellPeriodicDamage(Packet packet, ClientGame game)
        {
            Guid guid = new Guid();
            packet.ReadGuidBitStreamInOrder(guid, 6, 4, 1);
            bool isAlive = packet.ReadBit();
            packet.ReadGuidBitStreamInOrder(guid, 7, 2, 3, 5, 0);

            Player player = game.GetPlayer(packet.ReadUInt32());
            packet.ReadGuidByteStreamInOrder(guid, 0, 1, 2, 3, 7, 5, 4, 6);
            byte damage = packet.ReadByte();

            if (isAlive)
                player.AttackCard(guid, damage, CombatLogTypes.SpellUsage);
            else
                player.DestroyCard(guid, damage, CombatLogTypes.SpellUsage);
        }
    }
}
