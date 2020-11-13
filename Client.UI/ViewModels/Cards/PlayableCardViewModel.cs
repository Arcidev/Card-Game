using Arci.Networking.Data;
using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Enums;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace Client.UI.ViewModels.Cards
{
    public class PlayableCardViewModel : CardViewModel
    {
        private readonly PlayableCard card;
        private SelectionType selectionType;

        protected override Card Card => card;

        public ObservableCollection<SpellAuraViewModel> Auras { get; }

        public bool HasAuras => Auras.Any();

        public UInt64 Guid => card.Guid;

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

        public PlayableCardViewModel(PlayableCard card)
        {
            this.card = card;

            Auras = new ObservableCollection<SpellAuraViewModel>(card.Auras.Select(x => new SpellAuraViewModel(x.SpellId, x.AuraText, x.AuraImagePath)));
            card.StatChanged += stat => OnPropertyChanged(stat.ToString());
            card.SpellAurasChanged += OnSpellAurasChanged;
            card.CardChanged += OnCardChanged;
            SetBackground();

            OnClickCmd = new AsyncCommandHandler(UseCardAction, () => SelectionType == SelectionType.BasicDamageAttackable || SelectionType == SelectionType.SpellUsable);
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
            Application.Current.Dispatcher.Invoke(() =>
            {
                foreach (var aura in Auras.Where(x => !card.Auras.Any(y => x.SpellId == y.SpellId)).ToList())
                    Auras.Remove(aura);

                foreach (var aura in card.Auras.Where(x => !Auras.Any(y => x.SpellId == y.SpellId)))
                    Auras.Add(new SpellAuraViewModel(aura.SpellId, aura.AuraText, aura.AuraImagePath));

                OnPropertyChanged(nameof(HasAuras));
            });
        }

        private async Task UseCardAction()
        {
            if (SelectionType != SelectionType.BasicDamageAttackable && SelectionType != SelectionType.SpellUsable)
                return;

            var packet = new Packet(CMSGPackets.CardAction).Builder()
                .WriteGuidBitStreamInOrder(card.Guid, 4, 3, 2, 7, 1, 6, 0, 5)
                .FlushBits()
                .WriteGuidByteStreamInOrder(card.Guid, 6, 2, 7, 1, 0)
                .Write((byte)SelectionType)
                .WriteGuidByteStreamInOrder(card.Guid, 5, 3, 4)
                .Build();

            await card.Player.Game.SendPacketAsync(packet);
        }
    }
}
