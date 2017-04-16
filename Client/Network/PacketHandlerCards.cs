using Arci.Networking.Data;
using Client.Data;
using Client.Enums;
using Client.Game;
using System;
using System.Collections.Generic;

namespace Client.Network
{
    public static partial class PacketHandler
    {
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

            var guids1 = new PacketGuid[count1];
            var guids2 = new PacketGuid[count2];

            for (var i = 0; i < count2; i++)
            {
                guids2[i] = new PacketGuid();
                packet.ReadGuidBitStreamInOrder(guids2[i], 1, 2, 7, 0, 5, 3, 4, 6);
            }

            for (var i = 0; i < count1; i++)
            {
                guids1[i] = new PacketGuid();
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
            var guids = new PacketGuid[cardsCount];
            for (byte i = 0; i < cardsCount; i++)
            {
                guids[i] = new PacketGuid();
                packet.ReadGuidBitStreamInOrder(guids[i], 7, 2, 1, 4, 5, 0, 6, 3);
            }

            var player = game.GetPlayer(packet.ReadUInt32());
            if (player == null)
                return;

            for (byte i = 0; i < cardsCount; i++)
                packet.ReadGuidByteStreamInOrder(guids[i], 2, 1, 7, 6, 0, 5, 3, 4);

            player.PutCardsOnDeck(Array.ConvertAll(guids, guid => (UInt64)guid));
        }

        // Handle SMSG_CARD_STAT_CHANGED packet
        private static void HandleCardStatChanged(Packet packet, ClientGame game)
        {
            var guid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(guid, 2, 6, 7, 1, 0, 3, 5, 4);
            packet.ReadGuidByteStreamInOrder(guid, 5, 7);
            sbyte value = packet.ReadSByte();
            packet.ReadGuidByteStreamInOrder(guid, 6, 3, 1);
            UInt32 playerId = packet.ReadUInt32();
            packet.ReadGuidByteStreamInOrder(guid, 0, 4, 2);
            CardStats cardStat = (CardStats)packet.ReadByte();

            game.GetPlayer(playerId).ModifyCardStat(guid, cardStat, value);
        }

        // Handle SMSG_CARD_HEALED packet
        private static void HandleCardHealed(Packet packet, ClientGame game)
        {
            var guid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(guid, 7, 2, 6, 1, 3, 0, 5, 4);

            packet.ReadGuidByteStreamInOrder(guid, 5, 2, 7, 1);
            Player player = game.GetPlayer(packet.ReadUInt32());
            byte health = packet.ReadByte();
            packet.ReadGuidByteStreamInOrder(guid, 4, 0, 3, 6);
            byte amount = packet.ReadByte();

            player.HealCard(guid, health, amount);
        }

        // Handle SMSG_MANA_REPLENISHMENT packet
        private static void HandleManaReplenishment(Packet packet, ClientGame game)
        {
            byte cardCount = packet.ReadByte();
            var guids = new PacketGuid[cardCount];

            for (byte i = 0; i < cardCount; i++)
            {
                guids[i] = new PacketGuid();
                packet.ReadGuidBitStreamInOrder(guids[i], 5, 0, 1, 2, 3, 7, 4, 6);
            }

            Player player = game.GetPlayer(packet.ReadUInt32());
            byte manaReplenished = packet.ReadByte();

            for (byte i = 0; i < cardCount; i++)
            {
                packet.ReadGuidByteStreamInOrder(guids[i], 2, 6, 0, 7, 1, 4, 3, 5);
                byte mana = packet.ReadByte();

                player.SetCardMana(guids[i], mana);
            }

            game.Chat.LogManaReplenishment(player, manaReplenished);
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
                var cardGuid = new PacketGuid();
                packet.ReadGuidBitStreamInOrder(cardGuid, 6, 2, 1, 7, 3, 0, 4, 5);
                packet.ReadGuidByteStreamInOrder(cardGuid, 2, 6, 7);
                UInt32 attackerId = packet.ReadUInt32();
                packet.ReadGuidByteStreamInOrder(cardGuid, 1, 3, 0);
                byte damage = packet.ReadByte();
                packet.ReadGuidByteStreamInOrder(cardGuid, 5, 4);

                Player opponent = game.GetOpponent(attackerId);
                if (result == AttackResult.CardAttacked)
                    opponent.AttackCard(cardGuid, damage, CombatLogTypes.BasicDamage, false);
                else
                    opponent.DestroyCard(cardGuid, damage, CombatLogTypes.BasicDamage, false);
            }
        }
    }
}
