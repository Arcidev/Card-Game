using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class SelectableCard : Card
    {
        public SelectableCard(UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
            : base(id, type, hp, damage, mana, defense) { }
    }
}
