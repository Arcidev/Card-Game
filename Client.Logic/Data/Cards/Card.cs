using Client.Logic.Data.Spells;
using Client.Logic.Enums;
using System;

namespace Client.Logic.Data.Cards
{
    public class Card
    {
        private byte health;
        private byte mana;
        private sbyte damageModifier;
        private sbyte defenseModifier;

        public event Action<CardStats> StatChanged;

        public UInt32 Id { get; }

        public string Name { get; set; }

        public CreatureTypes Type { get; set; }

        public byte Hp
        {
            get => health;
            set
            {
                health = value;
                StatChanged?.Invoke(CardStats.Health);
            }
        }

        public byte Damage { get; set; }

        public sbyte DamageModifier
        {
            get => damageModifier;
            set
            {
                damageModifier = value;
                StatChanged?.Invoke(CardStats.Damage);
            }
        }

        public byte DamageModified
        {
            get
            {
                if (damageModifier < 0)
                    if (Damage < -damageModifier)
                        return 0;

                return (byte)(Damage + damageModifier);
            }
        }

        public byte Mana
        {
            get => mana;
            set
            {
                mana = value;
                StatChanged?.Invoke(CardStats.Mana);
            }
        }

        public byte Defense { get; set; }

        public sbyte DefenseModifier
        {
            get => defenseModifier;
            set
            {
                defenseModifier = value;
                StatChanged?.Invoke(CardStats.Defense);
            }
        }

        public byte DefenseModified
        {
            get
            {
                if (defenseModifier < 0)
                    if (Defense < -defenseModifier)
                        return 0;

                return (byte)(Defense + defenseModifier);
            }
        }

        public string ImageUri { get; set; }

        public Spell Spell { get; set; }

        public Card(UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense, Spell spell)
        {
            Id = id;
            Type = type;
            health = hp;
            Damage = damage;
            this.mana = mana;
            Defense = defense;
            Spell = spell;
        }
    }
}
