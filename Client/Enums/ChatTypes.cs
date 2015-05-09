using System.ComponentModel;

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
