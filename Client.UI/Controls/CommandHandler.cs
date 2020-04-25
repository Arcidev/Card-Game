using System;
using System.Windows.Input;

namespace Client.UI.Controls
{
    public class CommandHandler : ICommand
    {
        public event EventHandler CanExecuteChanged;

        private readonly Action action;
        private readonly Func<bool> canExecute;

        public CommandHandler(Action action, Func<bool> canExecute = null)
        {
            this.action = action;
            this.canExecute = canExecute;
        }

        public bool CanExecute(object parameter)
        {
            return canExecute?.Invoke() ?? true;
        }

        public void Execute(object parameter)
        {
            action.Invoke();
        }

        public void NotifyCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, new EventArgs());
        }
    }
}
