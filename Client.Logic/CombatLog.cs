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
    }
}
