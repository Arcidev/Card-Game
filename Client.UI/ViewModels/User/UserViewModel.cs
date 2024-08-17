using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.User
{
    public class UserViewModel
    {
        private readonly Game game;
        
        public string Name { get; }
        public AsyncCommandHandler AcceptFriend { get; }
        public AsyncCommandHandler DenyFriend { get; }
        public AsyncCommandHandler BlockUser { get; }
        public AsyncCommandHandler RemoveBlockedUser { get; }

        public UserViewModel(string name, Game game)
        {
            this.game = game;
            Name = name;
            AcceptFriend = new AsyncCommandHandler(() => HandleFriendRequest(UserRelationAction.AcceptFriend));
            DenyFriend = new AsyncCommandHandler(() => HandleFriendRequest(UserRelationAction.DenyFriend));
            BlockUser = new AsyncCommandHandler(() => HandleFriendRequest(UserRelationAction.BlockUser));
            RemoveBlockedUser = new AsyncCommandHandler(() => HandleFriendRequest(UserRelationAction.RemoveBlockedUser));
        }

        protected Task HandleFriendRequest(UserRelationAction action)
        {
            var packet = new Packet(CMSGPackets.UserRelation);
            packet.Write(Name);
            packet.Write((byte)action);

            return game.SendPacketAsync(packet);
        }
    }
}
