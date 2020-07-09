using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using System;
using System.Windows.Input;

namespace Client.UI.ViewModels.Cards
{
    public abstract class CardViewModel : NotifyPropertyViewModel
    {
        private string background;
        private ICommand onClickCmd;

        protected abstract Card Card { get; }

        public UInt32 Id => Card.Id;

        public byte Health => Card.Health;

        public byte Damage => Card.DamageModified;

        public byte Defense => Card.DefenseModified;

        public byte Mana => Card.Mana;

        public string Image => $"/Assets/Images/Cards/{Card.ImageUri}";

        public string Name => Card.Name;

        public bool HasSpell => Card.Spell != null;

        public string Spell => Card.Spell?.Info;

        public string Background
        {
            get => background;
            private set
            {
                if (background == value)
                    return;

                background = value;
                OnPropertyChanged();
            }
        }

        public ICommand OnClickCmd
        {
            get => onClickCmd;
            protected set
            {
                if (onClickCmd == value)
                    return;

                onClickCmd = value;
                OnPropertyChanged();
            }
        }

        protected void SetBackground()
        {
            switch (Card.Type)
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
