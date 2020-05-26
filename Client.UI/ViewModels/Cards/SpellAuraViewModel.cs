using System;

namespace Client.UI.ViewModels.Cards
{
    public class SpellAuraViewModel : NotifyPropertyViewModel
    {
        public UInt32 SpellId { get; }

        public string Text { get; }

        public string Image { get; }

        public SpellAuraViewModel(UInt32 spellId, string text, string imageUri)
        {
            SpellId = spellId;
            Text = text;
            Image = $"/Assets/Images/Spells/{imageUri}";
        }
    }
}
