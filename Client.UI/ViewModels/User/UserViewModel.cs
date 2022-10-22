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
            AcceptFriend = new AsyncCommandHandler(async () => await HandleFriendRequest(UserRelationAction.AcceptFriend));
            DenyFriend = new AsyncCommandHandler(async () => await HandleFriendRequest(UserRelationAction.DenyFriend));
            BlockUser = new AsyncCommandHandler(async () => await HandleFriendRequest(UserRelationAction.BlockUser));
            RemoveBlockedUser = new AsyncCommandHandler(async () => await HandleFriendRequest(UserRelationAction.RemoveBlockedUser));
        }

        protected async Task HandleFriendRequest(UserRelationAction action)
        {
            var packet = new Packet(CMSGPackets.UserRelation);
            packet.Write(Name);
            packet.Write((byte)action);

            await game.SendPacketAsync(packet);
        }
    }
}
