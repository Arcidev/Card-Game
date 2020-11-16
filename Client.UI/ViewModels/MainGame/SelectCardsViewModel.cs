using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.ViewModels.Cards;
using System;
using System.Collections;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.MainGame
{
    public class SelectCardsViewModel : NotifyPropertyViewModel
    {
        private IList selectedItems;
        private readonly Game game;

        public ObservableCollection<SelectableCardViewModel> Cards { get; }

        public CommandHandler SelectItemsCommand { get; }

        public AsyncCommandHandler SendSelectedCardsCommand { get; }

        public SelectCardsViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");

            Cards = new (game.DataHolder.Cards.Select(x => new SelectableCardViewModel(x)));
            SelectItemsCommand = new (SelectItems);
            SendSelectedCardsCommand = new (SendSelectedCards, () => selectedItems?.Count == 5);
        }

        private async Task SendSelectedCards()
        {
            var packet = new Packet(CMSGPackets.SelectedCards);
            var cards = selectedItems.Cast<SelectableCardViewModel>().ToList();

            packet.Write((byte)cards.Count);
            foreach (var card in cards)
                packet.Write(card.Id);

            await game.SendPacketAsync(packet);
        }

        private void SelectItems(object obj)
        {
            if (obj is not IList items)
                return;

            if (items.Count > 5)
                items.RemoveAt(0);

            selectedItems = items;
            SendSelectedCardsCommand.NotifyCanExecuteChanged();
        }
    }
}
