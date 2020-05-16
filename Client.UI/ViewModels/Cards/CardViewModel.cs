using Client.Logic.Data.Cards;
using Client.Logic.Enums;

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

        public string Name => card.Name;

        public bool HasSpell => card.Spell != null;

        public string Background { get; }

        public CardViewModel(T card)
        {
            this.card = card;
            card.StatChanged += stat => OnPropertyChanged(stat.ToString());

            switch (card.Type)
            {
                case CreatureType.Defensive:
                case CreatureType.Melee:
                    Background = "/Assets/Images/Cards/Template/gabriola-simplified.png";
                    break;
                case CreatureType.Ranged:
                    Background = HasSpell ? "/Assets/Images/Cards/Template/gabriola-simplified-b.png" : "/Assets/Images/Cards/Template/gabriola-simplified-g.png";
                    break;
            }
        }
    }
}
