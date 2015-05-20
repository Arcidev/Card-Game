using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class SpellEffect
    {
        private byte targetId;
        private byte[] effectValues;

        public static readonly byte SpellEffectsCount = 4;

        public SpellEffect(byte target, params byte[] effValues)
        {
            targetId = target;
            effectValues = effValues;
        }
    }
}
