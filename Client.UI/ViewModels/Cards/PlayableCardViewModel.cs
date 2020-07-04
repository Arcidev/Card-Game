using Client.Logic.Data.Cards;
using Client.UI.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.UI.ViewModels.Cards
{
    public class PlayableCardViewModel : CardViewModel<PlayableCard>
    {
        private SelectionType selectionType;

        public List<SpellAuraViewModel> Auras { get; }

        public SelectionType SelectionType
        {
            get => selectionType;
            set
            {
                if (selectionType == value)
                    return;

                selectionType = value;
                OnPropertyChanged();
            }
        }

        public UInt64 Guid => card.Guid;

        public PlayableCardViewModel(PlayableCard card) : base(card)
        {
            Auras = card.Auras.Select(x => new SpellAuraViewModel(x.SpellId, x.AuraText, x.AuraImagePath)).ToList();

            card.SpellAurasChanged += OnSpellAurasChanged;
            card.CardChanged += OnCardChanged;
        }

        private void OnCardChanged()
        {
            OnPropertyChanged(nameof(Image));
            OnPropertyChanged(nameof(Name));
            OnPropertyChanged(nameof(HasSpell));
            OnPropertyChanged(nameof(Spell));
            SetBackground();
        }

        private void OnSpellAurasChanged()
        {
            Auras.RemoveAll(x => !card.Auras.Any(y => x.SpellId == x.SpellId));
            foreach (var aura in card.Auras.Where(x => !Auras.Any(y => x.SpellId == x.SpellId)))
            {
                Auras.Add(new SpellAuraViewModel(aura.SpellId, aura.AuraText, aura.AuraImagePath));
            }

            OnPropertyChanged(nameof(Auras));
        }
    }
}
