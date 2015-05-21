using Client.Data;
using Client.Enums;
using Client.Misc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace Client.Game
{
    public class Player
    {
        private ClientGame game;
        private Dictionary<UInt64, PlayableCard> cards;
        private Pair<PlayableCard, Image>[] cardDeck;

        public UInt32 Id { get; set; }
        public string Name { get; set; }
        public bool IsActive { get; private set; }
        public int ActiveCardPosition { get; private set; }
        public PlayableCard ActiveCard { get { return cardDeck[ActiveCardPosition].First; } }
        public IEnumerable<PlayableCard> CardDeck { get { return cardDeck.Select(x => x.First); } }

        public Player(ClientGame clientGame, params Image[] imageLocations)
        {
            game = clientGame;
            cardDeck = new Pair<PlayableCard, Image>[imageLocations.Length];
            for (int i = 0; i < imageLocations.Length; i++)
            {
                cardDeck[i] = new Pair<PlayableCard, Image>();
                cardDeck[i].Second = imageLocations[i];
            }
        }

        // Adds cards to player card deck
        public void AddCards(IEnumerable<PlayableCard> cardsData)
        {
            cards = cardsData.ToDictionary(x => x.Guid);
        }

        // Invokes action to UI thread
        public void Invoke(Action action)
        {
            game.Invoke(action);
        }

        // put cards on deck
        public void PutCardsOnDeck(UInt64[] cardGuids)
        {
            for (int i = 0; i < cardGuids.Length; i++)
            {
                PlayableCard card;
                if (cards.TryGetValue(cardGuids[i], out card))
                {
                    Invoke(new Action(delegate()
                    {
                        cardDeck[i].First = card;
                        cardDeck[i].Second.Source = card.Image;
                    }));
                }
            }

            var availableCardsCount = cards.Count;
            for (int i = cardGuids.Length; i < cardDeck.Length; i++)
            {
                Invoke(new Action(delegate()
                {
                    cardDeck[i].First = null;
                    cardDeck[i].Second.Source = new BitmapImage(new Uri(i < availableCardsCount ? "Assets/CardBack.png" : "Assets/CardBackGrayscale.png", UriKind.Relative));
                }));
            }
        }

        // Selects possible targets attackable by basic attack on opponent deck for current player
        public void SetPossibleTargets(IEnumerable<UInt64> targetableCards)
        {
            if (!game.IsOpponent(Id))
                return;

            SelectPossibleTargets(targetableCards, SelectionType.BasicDamageAttackable);
        }

        // Selects active target
        public void SelectActiveCard()
        {
            var activeCardDeck = cardDeck[ActiveCardPosition];
            if (activeCardDeck.First.SelectionType != SelectionType.Selected)
            {
                Invoke(new Action(delegate()
                {
                    activeCardDeck.First.SelectionType = SelectionType.Selected;
                    activeCardDeck.Second.Source = activeCardDeck.First.Image;
                }));
            }
        }

        // Selects possible targets
        public void SelectPossibleTargets(IEnumerable<UInt64> targetableCards, SelectionType selection)
        {
            foreach (var card in cardDeck.Where(x => x.First != null && targetableCards.Contains(x.First.Guid)))
            {
                Invoke(new Action(delegate()
                {
                    card.First.SelectionType = selection;
                    card.Second.Source = card.First.Image;
                }));
            }
        }

        // Set active state
        public void SetActiveState(UInt64 cardGuid)
        {
            IsActive = true;
            for(int i = 0; i < cardDeck.Length; i++)
            {
                var c = cardDeck[i];
                if (c.First == null)
                    continue;

                var select = SelectionType.None;
                if (c.First.Guid == cardGuid)
                {
                    ActiveCardPosition = i;
                    select = SelectionType.Selected;
                }

                if (c.First.SelectionType != select)
                {
                    Invoke(new Action(delegate()
                    {
                        c.First.SelectionType = select;
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }

        // Sets waiting state
        public void SetWaitingState()
        {
            IsActive = false;
            foreach (var c in cardDeck)
            {
                if ((c.First != null) && (c.First.SelectionType != SelectionType.None))
                {
                    Invoke(new Action(delegate()
                    {
                        c.First.SelectionType = SelectionType.None;
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }

        // Gets card by control name
        public Pair<PlayableCard, Image> GetCardByImageControlName(string name)
        {
            return cardDeck.FirstOrDefault(x => x.Second.Name == name);
        }

        // Attacks card
        public void AttackCard(UInt64 guid, byte damage, CombatLogTypes combatLogType, bool isPeriodicDamage)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.First.Guid == guid);
            if (cardPair == null)
                return;

            if (isPeriodicDamage)
                game.Chat.LogPeriodicDamage(cardPair.First, damage, true);
            else
                game.Chat.LogDamage(combatLogType, game.GetOpponent(Id).ActiveCard, cardPair.First, damage, true);
            
            Invoke(new Action(delegate()
            {
                cardPair.First.Hp -= damage;
                cardPair.Second.Source = cardPair.First.Image;
            }));
        }

        // Destroys card
        public void DestroyCard(UInt64 guid, byte damage, CombatLogTypes combatLogType, bool isPeriodicDamage)
        {
            PlayableCard card = null;
            if (cards.TryGetValue(guid, out card))
            {
                if (isPeriodicDamage)
                    game.Chat.LogPeriodicDamage(card, damage, false);
                else
                    game.Chat.LogDamage(combatLogType, game.GetOpponent(Id).ActiveCard, card, damage, false);

                cards.Remove(guid);
            }
        }

        // Modifies card stat
        public void ModifyCardStat(UInt64 cardGuid, CardStats cardStat, sbyte value)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.First.Guid == cardGuid);
            if (cardPair == null)
                return;

            game.Chat.LogStatChange(cardStat, cardPair.First, value);
            Invoke(new Action(delegate()
            {
                cardPair.First.ApplyModifier(cardStat, value);
                cardPair.Second.Source = cardPair.First.Image;
            }));
        }

        // Checks if is posible to cast spell
        public bool CanCastSpell()
        {
            if (ActiveCard == null || ActiveCard.Spell == null)
                return false;

            return ActiveCard.Spell.ManaCost <= ActiveCard.Mana;
        }

        // Adds aura to creature
        public void ApplyAura(UInt64 cardGuid, UInt32 spellId)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.First.Guid == cardGuid);
            if (cardPair == null)
                return;

            SpellData spellData = DataHolder.GetSpellData(spellId);
            game.Chat.LogApplyAura(cardPair.First, spellData);

            /// TODO: add some graphics effect
        }

        public void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.First.Guid == cardGuid);
            if (cardPair == null)
                return;

            game.Chat.LogHeal(cardPair.First, amount);
            Invoke(new Action(delegate()
            {
                cardPair.First.Hp = health;
                cardPair.Second.Source = cardPair.First.Image;
            }));
        }
    }
}
