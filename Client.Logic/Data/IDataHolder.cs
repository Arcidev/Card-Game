using Client.Logic.Data.Achievements;
using Client.Logic.Data.Cards;
using Client.Logic.Data.Spells;
using System;
using System.Collections.Generic;

namespace Client.Logic.Data
{
    public interface IDataHolder
    {
        IEnumerable<SelectableCard> Cards { get; }

        void LoadData(IReadOnlyDictionary<UInt32, SelectableCard> cards);

        SelectableCard GetCard(UInt32 id);

        SpellData GetSpellData(UInt32 id);

        AchievementInfo GetAchievementInfo(UInt32 id);

        string GetCriteriaDescription(UInt32 id);
    }
}
