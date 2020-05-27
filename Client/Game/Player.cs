using Client.Data;
using Client.Enums;
using Client.Logic.Enums;
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
        private readonly (PlayableCard card, Image image)[] cardDeck;
        private Dictionary<UInt64, PlayableCard> cards;

        public UInt32 Id { get; set; }

        public string Name { get; set; }

        public bool IsActive { get; private set; }

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition].card;

        public IEnumerable<PlayableCard> CardDeck => cardDeck.Select(x => x.card);

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
                        cardDeck[i].card = card;
                        cardDeck[i].image.Source = card.Image;
                    });
                }
            }

            var availableCardsCount = cards.Count;
            for (var i = cardGuids.Length; i < cardDeck.Length; i++)
            {
                Invoke(() =>
                {
                    cardDeck[i].card = null;
                    cardDeck[i].image.Source = new BitmapImage(new Uri(i < availableCardsCount ? "Assets/CardBack.png" : "Assets/CardBackGrayscale.png", UriKind.Relative));
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
            if (activeCardDeck.card.SelectionType != SelectionType.Selected)
            {
                Invoke(() =>
                {
                    activeCardDeck.card.SelectionType = SelectionType.Selected;
                    activeCardDeck.image.Source = activeCardDeck.card.Image;
                });

                foreach(var card in cardDeck.Where(x => x.card != null))
                {
                    if (card.card.SelectionType == SelectionType.SpellUsable)
                    {
                        Invoke(() =>
                        {
                            card.card.SelectionType = SelectionType.None;
                            card.image.Source = card.card.Image;
                        });
                    }
                }
            }
        }

        // Remove selection
        public void RemoveSelectionFromCards()
        {
            foreach (var card in cardDeck.Where(x => x.card != null && x.card.SelectionType != SelectionType.None))
            {
                Invoke(() =>
                {
                    card.card.SelectionType = SelectionType.None;
                    card.image.Source = card.card.Image;
                });
            }
        }

        // Selects possible targets
        public void SelectPossibleTargets(IEnumerable<UInt64> targetableCards, SelectionType selection)
        {
            foreach (var card in cardDeck.Where(x => x.card != null && targetableCards.Contains(x.card.Guid)))
            {
                Invoke(() =>
                {
                    card.card.SelectionType = selection;
                    card.image.Source = card.card.Image;
                });
            }
        }

        // Set active state
        public void SetActiveState(UInt64 cardGuid)
        {
            IsActive = true;
            for(var i = 0; i < cardDeck.Length; i++)
            {
                var (card, image) = cardDeck[i];
                if (card == null)
                    continue;

                var select = SelectionType.None;
                if (card.Guid == cardGuid)
                {
                    ActiveCardPosition = i;
                    select = SelectionType.Selected;
                }

                if (card.SelectionType != select)
                {
                    Invoke(() =>
                    {
                        card.SelectionType = select;
                        image.Source = card.Image;
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
        public (PlayableCard card, Image image) GetCardByImageControlName(string name)
        {
            return cardDeck.FirstOrDefault(x => x.image.Name == name);
        }

        // Attacks card
        public void AttackCard(UInt64 guid, byte damage, CombatLogType combatLogType, bool isPeriodicDamage)
        {
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == guid);
            if (card == null)
                return;

            if (isPeriodicDamage)
                game.Chat.LogPeriodicDamage(card, damage, true);
            else
                game.Chat.LogDamage(combatLogType, game.GetOpponent(Id).ActiveCard, card, damage, true);
            
            Invoke(() =>
            {
                card.Hp -= damage;
                image.Source = card.Image;
            });
        }

        // Destroys card
        public void DestroyCard(UInt64 guid, byte damage, CombatLogType combatLogType, bool isPeriodicDamage)
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
        public void ModifyCardStat(UInt64 cardGuid, CardStat cardStat, sbyte value)
        {
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (card == null)
                return;

            game.Chat.LogStatChange(cardStat, card, value);
            Invoke(() =>
            {
                card.ApplyModifier(cardStat, value);
                image.Source = card.Image;
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
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (card == null)
                return;

            var spellData = DataHolder.GetSpellData(spellId);
            game.Chat.LogApplyAura(card, spellData);

            /// TODO: add some graphics effect
        }

        // Removes aura from creature
        public void ExpireAuras(UInt64 cardGuid, UInt32[] spellIds)
        {
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (card == null)
                return;

            foreach(var spellId in spellIds)
            {
                var spellData = DataHolder.GetSpellData(spellId);
                game.Chat.LogExpireAura(card, spellData);

                /// TODO: add some graphics effect
            }
        }

        // Heals card
        public void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (card == null)
                return;

            game.Chat.LogHeal(card, amount);
            Invoke(() =>
            {
                card.Hp = health;
                image.Source = card.Image;
            });
        }

        // Consumes mana from card and logs it into comba log
        public void HandleSuccessfulSpellCast(UInt64 cardGuid, UInt32 spellId, byte mana, byte manaCost)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (cardPair.card == null)
                return;

            var spellData = DataHolder.GetSpellData(spellId);
            game.Chat.LogManaConsume(cardPair.card, spellData, manaCost);

            SetCardMana(cardPair, mana);
        }

        // Sets cards mana
        public void SetCardMana(UInt64 cardGuid, byte mana)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (cardPair.card == null)
                return;

            SetCardMana(cardPair, mana);
        }

        // Morphs card
        public void MorphCard(UInt64 cardGuid, Card cardTemplate, byte mana, bool isMorph)
        {
            var (card, image) = cardDeck.FirstOrDefault(x => x.card?.Guid == cardGuid);
            if (card == null)
                return;

            game.Chat.LogMorph(card, cardTemplate, isMorph);
            Invoke(() =>
            {
                card.UnloadImages();
                card.Id = cardTemplate.Id;
                card.ImageUri = cardTemplate.ImageUri;
                card.Type = cardTemplate.Type;
                card.Spell = cardTemplate.Spell;
                card.Name = cardTemplate.Name;
                card.Damage = cardTemplate.Damage;
                card.Defense = cardTemplate.Defense;
                card.Mana = mana;
                image.Source = card.Image;
            });
        }

        // Sets card mana
        private void SetCardMana((PlayableCard card, Image image) cardPair, byte mana)
        {
            Invoke(() =>
            {
                cardPair.card.Mana = mana;
                cardPair.image.Source = cardPair.card.Image;
            });
        }
    }
}
