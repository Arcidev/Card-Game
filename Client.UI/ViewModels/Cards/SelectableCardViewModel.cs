using Client.Logic.Data.Cards;

namespace Client.UI.ViewModels.Cards
{
    public class SelectableCardViewModel : CardViewModel
    {
        private readonly SelectableCard card;

        protected override Card Card => card;

        public byte Price => card.Price;

        public SelectableCardViewModel(SelectableCard card)
        {
            this.card = card;
            SetBackground();
        }
    }
}
