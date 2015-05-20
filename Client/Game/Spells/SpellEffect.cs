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

        public SpellEffect(byte target)
        {
            targetId = target;
        }
    }
}
