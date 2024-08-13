using Client.UI.Enums;

namespace Client.UI.ViewModels.MainGame
{
    public record ChatMessageViewModel(string Author, string Message, ChatMessageType Type =  ChatMessageType.Message)
    {
        public bool ShowAuthor => Type == ChatMessageType.Message;
    }
}
