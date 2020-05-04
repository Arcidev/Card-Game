using Client.Logic.Data.Spells;
using Client.Logic.Enums;
using System;

namespace Client.Logic.Data.Cards
{
    public class SelectableCard : Card
    {
        public byte Price { get; }

        public SelectableCard(UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense, byte price, Spell spell)
            : base(id, type, hp, damage, mana, defense, spell) 
        {
            Price = price;
        }
    }
}
