using Client.Logic.Data.Cards;

namespace Client.UI.ViewModels.Cards
{
    public abstract class CardViewModel<T> : NotifyPropertyViewModel where T : Card
    {
        protected readonly T card;

        public byte Health => card.Health;

        public byte Damage => card.DamageModified;

        public byte Defense => card.DefenseModified;

        public byte Mana => card.Mana;

        public string ImageUri => card.ImageUri;

        public CardViewModel(T card)
        {
            this.card = card;
            card.StatChanged += stat => OnPropertyChanged(stat.ToString());
        }
    }
}
