using Client.Logic.Data;
using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic
{
    public class Player
    {
        private readonly Game game;
        private readonly PlayableCard[] cardDeck;
        private readonly Dictionary<UInt64, PlayableCard> cards;

        public event Action CardDeckChanged;

        public UInt32 Id { get; set; }

        public string Name { get; }

        public IEnumerable<PlayableCard> CardDeck => cardDeck;

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition];

        public Player(UInt32 id, string name, Game game)
        {
            Id = id;
            Name = name;
            this.game = game;

            cards = new Dictionary<UInt64, PlayableCard>();
            cardDeck = new PlayableCard[4];
        }

        internal void ModifyCardStat(UInt64 cardGuid, CardStat cardStat, sbyte value)
        {
            InvokeCardAction(cardGuid, card =>
            {
                card.ApplyModifier(cardStat, value);
                game.CombatLog.LogStatChange(cardStat, card.Name, value);

            });
        }

        internal void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            InvokeCardAction(cardGuid, card =>
            {
                card.Health = health;
                game.CombatLog.LogHeal(card.Name, amount);
            });
            
        }

        internal void SetCardMana(UInt64 cardGuid, byte mana)
        {
            InvokeCardAction(cardGuid, card => card.Mana = mana);
        }

        internal void MorphCard(UInt64 cardGuid, Card cardTemplate, byte mana, bool isMorph)
        {
            InvokeCardAction(cardGuid, card =>
            {
                game.CombatLog.LogMorph(card.Name, cardTemplate.Name, isMorph);
                card.Morph(cardTemplate, mana);
            });
        }

        internal void ApplyAura(UInt64 cardGuid, UInt32 spellId)
        {
            InvokeCardAction(cardGuid, card =>
            {
                var spellData = DataHolder.GetSpellData(spellId);
                if (spellData == null)
                    return;

                card.ApplyAura(spellData);
                game.CombatLog.LogApplyAura(card.Name, spellData.Name);
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

                    game.CombatLog.LogExpireAura(card.Name, spellData.Name);
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

        private void InvokeCardAction(UInt64 cardGuid, Action<PlayableCard> action)
        {
            var card = cardDeck.FirstOrDefault(x => x.Guid == cardGuid);
            if (card == null)
                return;

            action.Invoke(card);
        }
    }
}
