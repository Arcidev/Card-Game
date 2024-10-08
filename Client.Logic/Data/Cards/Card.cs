﻿using Client.Logic.Data.Spells;
using Client.Logic.Enums;
using System;

namespace Client.Logic.Data.Cards
{
    public abstract class Card
    {
        private byte health;
        private byte mana;
        private byte damage;
        private byte defense;
        private sbyte damageModifier;
        private sbyte defenseModifier;
        protected UInt32 id;

        public event Action<CardStat> StatChanged;

        public UInt32 Id => id;

        public string Name { get; set; }

        public CreatureType Type { get; set; }

        public byte Health
        {
            get => health;
            set
            {
                health = value;
                StatChanged?.Invoke(CardStat.Health);
            }
        }

        public byte Damage
        {
            get => damage;
            set
            {
                damage = value;
                StatChanged?.Invoke(CardStat.Damage);
            }
        }

        public sbyte DamageModifier
        {
            get => damageModifier;
            set
            {
                damageModifier = value;
                StatChanged?.Invoke(CardStat.Damage);
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
                StatChanged?.Invoke(CardStat.Mana);
            }
        }

        public byte Defense
        {
            get => defense;
            set
            {
                defense = value;
                StatChanged?.Invoke(CardStat.Defense);
            }
        }

        public sbyte DefenseModifier
        {
            get => defenseModifier;
            set
            {
                defenseModifier = value;
                StatChanged?.Invoke(CardStat.Defense);
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

        protected Card(UInt32 id, CreatureType type, byte health, byte damage, byte mana, byte defense, Spell spell)
        {
            this.id = id;
            Type = type;
            this.health = health;
            this.damage = damage;
            this.mana = mana;
            this.defense = defense;
            Spell = spell;
        }
    }
}
