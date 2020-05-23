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
    }
}
