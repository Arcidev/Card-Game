using Client.Logic.Data.Cards;
using Client.Logic.Data.Spells;
using System;
using System.Collections.Generic;

namespace Client.Logic.Data
{
    public interface IDataHolder
    {
        void LoadData(IDictionary<UInt32, SelectableCard> cards);

        IEnumerable<SelectableCard> Cards { get; }

        SelectableCard GetCard(UInt32 id);

        SpellData GetSpellData(UInt32 id);
    }
}
