﻿using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace Client.UI.ViewModels.User
{
    public class UserListViewModel : IDisposable
    {
        public ObservableCollection<FriendViewModel> Friends { get; } = [];

        public ObservableCollection<UserViewModel> FriendRequests { get; } = [];

        public ObservableCollection<UserViewModel> BlockedUsers { get; } = [];

        public AsyncCommandHandler AddFriendCmd { get; }

        public AsyncCommandHandler BlockUserCmd { get; }

        public UserListViewModel()
        {
            var game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            game.PropertyChanged += OnGamePropertyChanged;

            ReloadFriends();
            ReloadFriendRequests();
            ReloadBlockedUsers();

            AddFriendCmd = new (async (o) => await HandleUser(o as string, UserRelationAction.AddFriend));
            BlockUserCmd = new (async (o) => await HandleUser(o as string, UserRelationAction.BlockUser));
        }

        public void Dispose()
        {
            App.GetGame().PropertyChanged -= OnGamePropertyChanged;
            GC.SuppressFinalize(this);
        }

        private void OnGamePropertyChanged(string property)
        {
            switch (property)
            {
                case nameof(Game.Friends):
                    ReloadFriends();
                    break;
                case nameof(Game.BlockedUsers):
                    ReloadBlockedUsers();
                    break;
                case nameof(Game.FriendRequets):
                    ReloadFriendRequests();
                    break;
            }
        }

        private void ReloadFriends()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                Friends.Clear();
                foreach (var friend in game.Friends?.Select(x => new FriendViewModel(x.name, x.isOnline, game)) ?? [])
                    Friends.Add(friend);
            });
        }

        private void ReloadFriendRequests()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                FriendRequests.Clear();
                foreach (var friendRequest in game.FriendRequets?.Select(x => new UserViewModel(x, game)) ?? [])
                    FriendRequests.Add(friendRequest);
            });
        }

        private void ReloadBlockedUsers()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                BlockedUsers.Clear();
                foreach (var blockedUser in game.BlockedUsers?.Select(x => new UserViewModel(x, game)) ?? [])
                    BlockedUsers.Add(blockedUser);
            });
        }

        private static Task HandleUser(string name, UserRelationAction action)
        {
            if (string.IsNullOrEmpty(name))
                return Task.CompletedTask;

            var game = App.GetGame();
            var packet = new Packet((UInt16)CMSGPackets.UserRelation).Builder()
                .Write(name).Write((byte)action).Build();

            return game.SendPacketAsync(packet);
        }
    }
}
