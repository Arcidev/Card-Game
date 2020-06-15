using Client.Logic;
using Client.UI.ViewModels.Cards;
using System.Collections.ObjectModel;
using System.Linq;

namespace Client.UI.ViewModels.MainGame
{
    public class CardDeckViewModel : NotifyPropertyViewModel
    {
        private readonly Player player;

        public ObservableCollection<PlayableCardViewModel> Cards { get; }

        public CardDeckViewModel(Player player)
        {
            this.player = player;

            Cards = new ObservableCollection<PlayableCardViewModel>(player.CardDeck.Where(x => x != null).Select(x => new PlayableCardViewModel(x)));

            player.CardDeckChanged += OnDeckChanged;
        }

        private void OnDeckChanged()
        {
            Cards.Clear();
            foreach (var card in player.CardDeck.Where(x => x != null).Select(x => new PlayableCardViewModel(x)))
                Cards.Add(card);
        }
    }
}
