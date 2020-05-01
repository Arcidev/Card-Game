using System.ComponentModel;

namespace Client.Logic.Enums
{
    public enum ChatType
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
