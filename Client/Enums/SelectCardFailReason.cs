using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Enums
{
    public enum SelectCardFailReason
    {
        [Description("Invalid card count")]
        InvalidCardCount = 1,
        [Description("Server does not support every card you've selected")]
        InvalidCardId
    }
}
