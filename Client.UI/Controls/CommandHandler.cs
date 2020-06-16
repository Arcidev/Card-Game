using System;
using System.Windows.Input;

namespace Client.UI.Controls
{
    public class CommandHandler : ICommand
    {
        public event EventHandler CanExecuteChanged;

        private readonly Action<object> action;
        private readonly Func<object, bool> canExecute;

        public CommandHandler(Action action, Func<bool> canExecute = null) : this(o => action())
        {
            if (canExecute != null)
                this.canExecute = o => canExecute();
        }

        public CommandHandler(Action<object> action, Func<object, bool> canExecute = null)
        {
            this.action = action;
            this.canExecute = canExecute;
        }

        public bool CanExecute(object parameter)
        {
            return canExecute?.Invoke(parameter) ?? true;
        }

        public void Execute(object parameter)
        {
            action.Invoke(parameter);
        }

        public void NotifyCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, new EventArgs());
        }
    }
}
