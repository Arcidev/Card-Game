using Client.Logic.Data.Cards;

namespace Client.UI.ViewModels.Cards
{
    public class SelectableCardViewModel : CardViewModel<SelectableCard>
    {
        public byte Price => card.Price;

        public SelectableCardViewModel(SelectableCard card) : base(card)
        {
        }
    }
}
