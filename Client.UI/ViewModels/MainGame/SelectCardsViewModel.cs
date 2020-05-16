using Client.Logic;
using Client.Logic.Data;
using Client.UI.ViewModels.Cards;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace Client.UI.ViewModels.MainGame
{
    public class SelectCardsViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public ObservableCollection<SelectableCardViewModel> Cards { get; }

        public SelectCardsViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");

            Cards = new ObservableCollection<SelectableCardViewModel>(DataHolder.Cards.Select(x => new SelectableCardViewModel(x)));
        }
    }
}
