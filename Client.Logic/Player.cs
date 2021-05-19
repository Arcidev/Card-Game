using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic
{
    public class Player
    {
        private readonly PlayableCard[] cardDeck;
        private readonly Dictionary<UInt64, PlayableCard> cards;

        public event Action CardDeckChanged;

        public UInt32 Id { get; set; }

        public string Name { get; }

        public Game Game { get; }

        public IEnumerable<PlayableCard> CardDeck => cardDeck;

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition];

        public Player(UInt32 id, string name, Game game)
        {
            Id = id;
            Name = name;
            Game = game;

            cards = new Dictionary<UInt64, PlayableCard>();
            cardDeck = new PlayableCard[4];
        }

        internal void ModifyCardStat(UInt64 cardGuid, CardStat cardStat, sbyte value)
        {
            InvokeCardAction(cardGuid, card =>
            {
                card.ApplyModifier(cardStat, value);
                Game.CombatLog.LogStatChange(cardStat, card.Name, value);

            });
        }

        internal void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            InvokeCardAction(cardGuid, card =>
            {
                card.Health = health;
                Game.CombatLog.LogHeal(card.Name, amount);
            });
            
        }

        internal void SetCardMana(UInt64 cardGuid, byte mana) => InvokeCardAction(cardGuid, card => card.Mana = mana);

        internal void MorphCard(UInt64 cardGuid, Card cardTemplate, byte mana, bool isMorph)
        {
            InvokeCardAction(cardGuid, card =>
            {
                Game.CombatLog.LogMorph(card.Name, cardTemplate.Name, isMorph);
                card.Morph(cardTemplate, mana);
            });
        }

        internal void ApplyAura(UInt64 cardGuid, UInt32 spellId)
        {
            InvokeCardAction(cardGuid, card =>
            {
                var spellData = Game.DataHolder.GetSpellData(spellId);
                if (spellData == null)
                    return;

                card.ApplyAura(spellData);
                Game.CombatLog.LogApplyAura(card.Name, spellData.Name);
            });
        }

        internal void ExpireAuras(UInt64 cardGuid, uint[] spellIds)
        {
            InvokeCardAction(cardGuid, card =>
            {
                foreach (var spellId in spellIds)
                {
                    var spellData = card.RemoveAura(spellId);
                    if (spellData == null)
                        continue;

                    Game.CombatLog.LogExpireAura(card.Name, spellData.Name);
                }
            });
        }

        internal void SetCards(IEnumerable<PlayableCard> cardsData)
        {
            cards.Clear();
            foreach (var card in cardsData)
                cards.Add(card.Guid, card);
        }

        internal void PutCardsOnDeck(UInt64[] cardGuids)
        {
            for (var i = 0; i < cardGuids.Length; i++)
            {
                if (cards.TryGetValue(cardGuids[i], out var card))
                    cardDeck[i] = card;
                else
                    cardDeck[i] = null;
            }

            for (var i = cardGuids.Length; i < cardDeck.Length; i++)
                cardDeck[i] = null;

            CardDeckChanged?.Invoke();
        }


        internal void AttackCard(UInt64 guid, byte damage, CombatLogType combatLogType, bool isPeriodicDamage)
        {
            InvokeCardAction(guid, card =>
            {
                card.Health -= damage;

                if (isPeriodicDamage)
                    Game.CombatLog.LogPeriodicDamage(card.Name, damage, true);
                else
                    Game.CombatLog.LogDamage(combatLogType, Game.GetOpponent(Id).ActiveCard?.Name, card.Name, damage, true);
            });
        }

        internal void DestroyCard(UInt64 guid, byte damage, CombatLogType combatLogType, bool isPeriodicDamage)
        {
            if (cards.TryGetValue(guid, out var card))
            {
                if (isPeriodicDamage)
                    Game.CombatLog.LogPeriodicDamage(card.Name, damage, false);
                else
                    Game.CombatLog.LogDamage(combatLogType, Game.GetOpponent(Id).ActiveCard?.Name, card.Name, damage, false);

                cards.Remove(guid);
            }
        }

        internal void SetActiveCard(UInt64 cardGuid)
        {
            for (var i = 0; i < cardDeck.Length; i++)
            {
                if (cardDeck[i]?.Guid == cardGuid)
                {
                    ActiveCardPosition = i;
                    break;
                }
            }
        }

        internal PlayableCard GetCard(UInt64 cardGuid) => cardDeck.FirstOrDefault(x => x?.Guid == cardGuid);

        internal int GetCardIndex(PlayableCard card) => Array.IndexOf(cardDeck, card);

        internal void HandleSuccessfulSpellCast(UInt64 cardGuid, UInt32 spellId, byte mana, byte manaCost)
        {
            InvokeCardAction(cardGuid, card =>
            {
                var spellData = Game.DataHolder.GetSpellData(spellId);
                Game.CombatLog.LogManaConsume(card.Name, spellData.Name, manaCost);
                card.Mana = mana;
            });
        }

        internal void SetCard(PlayableCard card, int index)
        {
            if (index >= cardDeck.Length)
                throw new ArgumentOutOfRangeException(nameof(index));

            cardDeck[index] = card ?? throw new ArgumentNullException(nameof(card));
            card.Player = this;
            CardDeckChanged?.Invoke();
        }

        private void InvokeCardAction(UInt64 cardGuid, Action<PlayableCard> action)
        {
            var card = Game.GetCard(cardGuid);
            if (card == null)
                return;

            action.Invoke(card);
        }
    }
}
