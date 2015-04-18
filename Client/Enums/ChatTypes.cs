using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Enums
{
    public enum ChatTypes
    {
        Global = 1,
        Game,
        [Description("From")]
        Whisper,
        [Description("To")]
        WhisperResponse,
        Info,
        AutoSelect
    }
}
