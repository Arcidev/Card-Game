using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using System;

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
        }

        private void InvokeCardAction(CardAction action)
        {
        }
    }
}
