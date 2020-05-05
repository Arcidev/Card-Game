using Client.Logic.Data.Cards;

namespace Client.UI.ViewModels.Cards
{
    public class PlayableCardViewModel : CardViewModel<PlayableCard>
    {
        public PlayableCardViewModel(PlayableCard card) : base(card)
        {
        }
    }
}
