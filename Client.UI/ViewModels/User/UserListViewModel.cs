﻿using Client.Logic;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;

namespace Client.UI.ViewModels.User
{
    public class UserListViewModel : IDisposable
    {
        public ObservableCollection<FriendViewModel> Friends { get; } = new();

        public ObservableCollection<UserViewModel> FriendRequests { get; } = new();

        public ObservableCollection<UserViewModel> BlockedUsers { get; } = new();

        public UserListViewModel()
        {
            var game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            game.PropertyChanged += OnGamePropertyChanged;

            ReloadFriends();
            ReloadFriendRequests();
            ReloadBlockedUsers();
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
                    break;
            }
        }

        private void ReloadFriends()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                Friends.Clear();
                foreach (var friend in game.Friends?.Select(x => new FriendViewModel(x.name, x.isOnline, game)) ?? Enumerable.Empty<FriendViewModel>())
                    Friends.Add(friend);
            });
        }

        private void ReloadFriendRequests()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                FriendRequests.Clear();
                foreach (var friendRequest in game.FriendRequets?.Select(x => new UserViewModel(x, game)) ?? Enumerable.Empty<UserViewModel>())
                    FriendRequests.Add(friendRequest);
            });
        }

        private void ReloadBlockedUsers()
        {
            var game = App.GetGame();

            Application.Current.Dispatcher.Invoke(() =>
            {
                BlockedUsers.Clear();
                foreach (var blockedUser in game.BlockedUsers?.Select(x => new UserViewModel(x, game)) ?? Enumerable.Empty<UserViewModel>())
                    BlockedUsers.Add(blockedUser);
            });
        }
    }
}
