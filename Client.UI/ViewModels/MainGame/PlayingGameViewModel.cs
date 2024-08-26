using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Enums;
using System;
using System.Linq;
using System.Windows;

namespace Client.UI.ViewModels.MainGame
{
    public class PlayingGameViewModel : NotifyPropertyViewModel
    {
        public Game Game { get; }

        public CardDeckViewModel Player { get; }

        public CardDeckViewModel Opponent { get; }

        public CommandHandler AttackCmd { get; }

        public CommandHandler SpellAttackCmd { get; }

        public AsyncCommandHandler DefendCmd { get; }

        public PlayingGameViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            if (Game.Player == null || Game.Opponent == null)
                throw new InvalidOperationException("Both players must exist at this point");

            Player = new (Game.Player);
            Opponent = new (Game.Opponent);

            AttackCmd = new (() => InvokeCardAction(CardAction.BasicAttack));
            SpellAttackCmd = new (() => InvokeCardAction(CardAction.SpellUse), () => Game.Player.ActiveCard?.Spell != null);
            DefendCmd = new (Game.DefendSelfAsync);

            Game.PacketProcessed += OnPacketProcessed;
        }

        private void OnPacketProcessed(UInt16 packet)
        {
            if (packet != (UInt16)SMSGPackets.ActivePlayer)
                return;

            foreach (var card in Player.Cards)
                card.SelectionType = !Game.IsGameWaiting && Game.Player.ActiveCard.Guid == card.Guid ? SelectionType.Selected : SelectionType.None;

            foreach (var card in Opponent.Cards)
                card.SelectionType = SelectionType.None;

            Application.Current.Dispatcher.Invoke(SpellAttackCmd.NotifyCanExecuteChanged);
        }

        private void InvokeCardAction(CardAction action)
        {
            if (action == CardAction.SpellUse)
            {
                var targets = Game.Player.ActiveCard.Spell?.GetPossibleTargets(Game.Player, Game.Opponent);
                if (targets == null)
                    return;

                foreach (var playerCards in Player.Cards.Where(x => targets.Contains(x.Guid)).Concat(Opponent.Cards.Where(x => targets.Contains(x.Guid))))
                {
                    playerCards.SelectionType = SelectionType.SpellUsable;
                }
            }
            else if (action == CardAction.BasicAttack)
            {
                var targets = Game.Player.ActiveCard.GetPossibleTargets(Game.Opponent.CardDeck, Game.Player.ActiveCardPosition);
                foreach (var playerCards in Opponent.Cards.Where(x => targets.Contains(x.Guid)))
                {
                    playerCards.SelectionType = SelectionType.BasicDamageAttackable;
                }
            }
        }
    }
}
