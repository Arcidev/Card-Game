using Client.UI.Enums;

namespace Client.UI.ViewModels.MainGame
{
    public record ChatMessageViewModel(string Author, string Message, ChatMessageType Type)
    {
        public string Text => Type switch
        {
            ChatMessageType.Message => $"{Author}>\u00A0{Message}",
            _ => Message,
        };
    }
}
