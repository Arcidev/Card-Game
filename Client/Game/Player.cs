using Client.Data;
using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace Client.Game
{
    public class Player
    {
        private readonly ClientGame game;
        private readonly ValueTuple<PlayableCard, Image>[] cardDeck;
        private Dictionary<UInt64, PlayableCard> cards;

        public UInt32 Id { get; set; }

        public string Name { get; set; }

        public bool IsActive { get; private set; }

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition].Item1;

        public IEnumerable<PlayableCard> CardDeck => cardDeck.Select(x => x.Item1);

        public Player(ClientGame clientGame, params Image[] imageLocations)
        {
            game = clientGame;
            cardDeck = new (PlayableCard, Image)[imageLocations.Length];
            for (var i = 0; i < imageLocations.Length; i++)
                cardDeck[i] = (null, imageLocations[i]);
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
            for (var i = 0; i < cardGuids.Length; i++)
            {
                if (cards.TryGetValue(cardGuids[i], out var card))
                {
                    Invoke(() =>
                    {
                        cardDeck[i].Item1 = card;
                        cardDeck[i].Item2.Source = card.Image;
                    });
                }
            }

            var availableCardsCount = cards.Count;
            for (var i = cardGuids.Length; i < cardDeck.Length; i++)
            {
                Invoke(() =>
                {
                    cardDeck[i].Item1 = null;
                    cardDeck[i].Item2.Source = new BitmapImage(new Uri(i < availableCardsCount ? "Assets/CardBack.png" : "Assets/CardBackGrayscale.png", UriKind.Relative));
                });
            }
        }

        // Selects possible targets attackable by basic attack on opponent deck for current player
        public void SetPossibleTargets(IEnumerable<UInt64> targetableCards)
        {
            if (!game.IsOpponent(Id))
                return;

            SelectPossibleTargets(targetableCards, SelectionType.BasicDamageAttackable);
        }

        // Deselects friendly spell targets
        public void DeselectSpellFriendlyTargets()
        {
            var activeCardDeck = cardDeck[ActiveCardPosition];
            if (activeCardDeck.Item1.SelectionType != SelectionType.Selected)
            {
                Invoke(() =>
                {
                    activeCardDeck.Item1.SelectionType = SelectionType.Selected;
                    activeCardDeck.Item2.Source = activeCardDeck.Item1.Image;
                });

                foreach(var card in cardDeck.Where(x => x.Item1 != null))
                {
                    if (card.Item1.SelectionType == SelectionType.SpellUsable)
                    {
                        Invoke(() =>
                        {
                            card.Item1.SelectionType = SelectionType.None;
                            card.Item2.Source = card.Item1.Image;
                        });
                    }
                }
            }
        }

        // Remove selection
        public void RemoveSelectionFromCards()
        {
            foreach (var card in cardDeck.Where(x => x.Item1 != null && x.Item1.SelectionType != SelectionType.None))
            {
                Invoke(() =>
                {
                    card.Item1.SelectionType = SelectionType.None;
                    card.Item2.Source = card.Item1.Image;
                });
            }
        }

        // Selects possible targets
        public void SelectPossibleTargets(IEnumerable<UInt64> targetableCards, SelectionType selection)
        {
            foreach (var card in cardDeck.Where(x => x.Item1 != null && targetableCards.Contains(x.Item1.Guid)))
            {
                Invoke(() =>
                {
                    card.Item1.SelectionType = selection;
                    card.Item2.Source = card.Item1.Image;
                });
            }
        }

        // Set active state
        public void SetActiveState(UInt64 cardGuid)
        {
            IsActive = true;
            for(var i = 0; i < cardDeck.Length; i++)
            {
                var c = cardDeck[i];
                if (c.Item1 == null)
                    continue;

                var select = SelectionType.None;
                if (c.Item1.Guid == cardGuid)
                {
                    ActiveCardPosition = i;
                    select = SelectionType.Selected;
                }

                if (c.Item1.SelectionType != select)
                {
                    Invoke(() =>
                    {
                        c.Item1.SelectionType = select;
                        c.Item2.Source = c.Item1.Image;
                    });
                }
            }
        }

        // Sets waiting state
        public void SetWaitingState()
        {
            IsActive = false;
            RemoveSelectionFromCards();
        }

        // Gets card by control name
        public (PlayableCard, Image) GetCardByImageControlName(string name)
        {
            return cardDeck.FirstOrDefault(x => x.Item2.Name == name);
        }

        // Attacks card
        public void AttackCard(UInt64 guid, byte damage, CombatLogTypes combatLogType, bool isPeriodicDamage)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == guid);
            if (cardPair.Item1 == null)
                return;

            if (isPeriodicDamage)
                game.Chat.LogPeriodicDamage(cardPair.Item1, damage, true);
            else
                game.Chat.LogDamage(combatLogType, game.GetOpponent(Id).ActiveCard, cardPair.Item1, damage, true);
            
            Invoke(() =>
            {
                cardPair.Item1.Hp -= damage;
                cardPair.Item2.Source = cardPair.Item1.Image;
            });
        }

        // Destroys card
        public void DestroyCard(UInt64 guid, byte damage, CombatLogTypes combatLogType, bool isPeriodicDamage)
        {
            if (cards.TryGetValue(guid, out var card))
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
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == cardGuid);
            if (cardPair.Item1 == null)
                return;

            game.Chat.LogStatChange(cardStat, cardPair.Item1, value);
            Invoke(() =>
            {
                cardPair.Item1.ApplyModifier(cardStat, value);
                cardPair.Item2.Source = cardPair.Item1.Image;
            });
        }

        // Checks if is posible to cast spell
        public bool CanCastSpell()
        {
            return ActiveCard?.Spell?.ManaCost <= ActiveCard.Mana;
        }

        // Adds aura to creature
        public void ApplyAura(UInt64 cardGuid, UInt32 spellId)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == cardGuid);
            if (cardPair.Item1 == null)
                return;

            var spellData = DataHolder.GetSpellData(spellId);
            game.Chat.LogApplyAura(cardPair.Item1, spellData);

            /// TODO: add some graphics effect
        }

        // Heals card
        public void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == cardGuid);
            if (cardPair.Item1 == null)
                return;

            game.Chat.LogHeal(cardPair.Item1, amount);
            Invoke(() =>
            {
                cardPair.Item1.Hp = health;
                cardPair.Item2.Source = cardPair.Item1.Image;
            });
        }

        // Consumes mana from card and logs it into comba log
        public void HandleSuccessfulSpellCast(UInt64 cardGuid, UInt32 spellId, byte mana, byte manaCost)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == cardGuid);
            if (cardPair.Item1 == null)
                return;

            var spellData = DataHolder.GetSpellData(spellId);
            game.Chat.LogManaConsume(cardPair.Item1, spellData, manaCost);

            SetCardMana(cardPair, mana);
        }

        // Sets cards mana
        public void SetCardMana(UInt64 cardGuid, byte mana)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.Item1?.Guid == cardGuid);
            if (cardPair.Item1 == null)
                return;

            SetCardMana(cardPair, mana);
        }

        // Sets card mana
        private void SetCardMana((PlayableCard, Image) cardPair, byte mana)
        {
            Invoke(() =>
            {
                cardPair.Item1.Mana = mana;
                cardPair.Item2.Source = cardPair.Item1.Image;
            });
        }
    }
}
