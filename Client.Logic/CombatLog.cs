using Client.Logic.Enums;
using Client.Logic.Resources;
using System;

namespace Client.Logic
{
    public class CombatLog
    {
        public event Action<CombatLogType, string> MessageReceived;

        internal void LogHeal(string name, byte amount)
        {
            MessageReceived?.Invoke(CombatLogType.SpellUsage, string.Format(Texts.CardHealed, name, amount));
        }

        internal void LogStatChange(CardStat cardStat, string name, sbyte value)
        {
            MessageReceived?.Invoke(CombatLogType.StatChange, string.Format(Texts.StatChanged, name, cardStat.GetDescription(), value));
        }

        internal void LogManaReplenishment(Player player, byte manaReplenished)
        {
            MessageReceived?.Invoke(CombatLogType.StatChange, string.Format(Texts.ManaReplenished, player?.Name, manaReplenished));
        }

        internal void LogMorph(string card, string morph, bool isMorph)
        {
            var message = isMorph ? string.Format(Texts.CardMorphed, card, morph) : string.Format(Texts.CardDemorph, morph, card);
            MessageReceived?.Invoke(CombatLogType.SpellUsage, message);
        }

        internal void LogApplyAura(string card, string spell)
        {
            MessageReceived?.Invoke(CombatLogType.SpellUsage, string.Format(Texts.AuraApplied, card, spell));
        }

        internal void LogExpireAura(string card, string spell)
        {
            MessageReceived?.Invoke(CombatLogType.SpellUsage, string.Format(Texts.AuraExpired, spell, card));
        }

        internal void LogDamage(CombatLogType combatLogType, string attackerName, string victimName, byte damage, bool alive)
        {
            var message = alive ? $"{attackerName} dealt {damage} damage to {victimName}" : $"{attackerName} killed {victimName} with {damage}";
            MessageReceived?.Invoke(combatLogType, message);
        }

        internal void LogPeriodicDamage(string name, byte damage, bool alive)
        {
            var message = alive ? $"{name} suffered {damage} damage" : $"{name} has been killed with {damage} damage";
            MessageReceived?.Invoke(CombatLogType.SpellUsage, message);
        }

        internal void LogManaConsume(string card, string spell, byte manaCost)
        {
            MessageReceived?.Invoke(CombatLogType.SpellUsage, $"{spell} consumes {manaCost} mana from {card}");
        }
    }
}
