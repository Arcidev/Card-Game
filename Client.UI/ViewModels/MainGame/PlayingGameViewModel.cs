using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Enums;
using System;
using System.Linq;

namespace Client.UI.ViewModels.MainGame
{
    public class PlayingGameViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public CardDeckViewModel Player { get; }

        public CardDeckViewModel Opponent { get; }

        public CommandHandler AttackCmd { get; }

        public CommandHandler SpellAttackCmd { get; }

        public AsyncCommandHandler DefendCmd { get; }

        public PlayingGameViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            if (game.Player == null || game.Opponent == null)
                throw new InvalidOperationException("Both players must exist at this point");

            Player = new CardDeckViewModel(game.Player);
            Opponent = new CardDeckViewModel(game.Opponent);

            AttackCmd = new CommandHandler(() => InvokeCardAction(CardAction.BasicAttack));
            SpellAttackCmd = new CommandHandler(() => InvokeCardAction(CardAction.SpellUse));
            DefendCmd = new AsyncCommandHandler(() => game.DefendSelfAsync());

            game.PacketProcessed += OnPacketProcessed;
        }

        private void OnPacketProcessed(UInt16 packet)
        {
            if (packet == (UInt16)SMSGPackets.ActivePlayer)
            {
                foreach (var card in Player.Cards)
                    card.SelectionType = !game.IsGameWaiting && game.Player.ActiveCard.Guid == card.Guid ? SelectionType.Selected : SelectionType.None;

                foreach (var card in Opponent.Cards)
                    card.SelectionType = SelectionType.None;
            }
        }

        private void InvokeCardAction(CardAction action)
        {
            if (action == CardAction.SpellUse)
            {
                var targets = game.Player.ActiveCard.Spell?.GetPossibleTargets(game.Player, game.Opponent);

                foreach (var playerCards in Player.Cards.Where(x => targets.Contains(x.Guid)).Concat(Opponent.Cards.Where(x => targets.Contains(x.Guid))))
                {
                    playerCards.SelectionType = SelectionType.SpellUsable;
                }
            }
            else if (action == CardAction.BasicAttack)
            {
                var targets = game.Player.ActiveCard?.GetPossibleTargets(game.Opponent.CardDeck, game.Player.ActiveCardPosition);
                foreach (var playerCards in Opponent.Cards.Where(x => targets.Contains(x.Guid)))
                {
                    playerCards.SelectionType = SelectionType.BasicDamageAttackable;
                }
            }
        }
    }
}
