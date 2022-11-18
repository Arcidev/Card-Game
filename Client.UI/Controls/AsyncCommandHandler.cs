using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Client.UI.Controls
{
    public class AsyncCommandHandler : ICommand
    {
        public event EventHandler CanExecuteChanged;
        public event Action OnExecuted;

        private bool isExecuting;
        private readonly Func<object, Task> execute;
        private readonly Func<object, bool> canExecute;

        public AsyncCommandHandler(Func<Task> execute, Func<bool> canExecute = null)
        {
            this.execute = (o) => execute();
            this.canExecute = (o) => canExecute?.Invoke() ?? true;
        }

        public AsyncCommandHandler(Func<object, Task> execute, Func<object, bool> canExecute = null)
        {
            this.execute = execute;
            this.canExecute = canExecute;
        }

        public bool CanExecute(object obj)
        {
            return !isExecuting && (canExecute?.Invoke(obj) ?? true);
        }

        public async Task ExecuteAsync(object obj)
        {
            if (CanExecute(obj))
            {
                try
                {
                    isExecuting = true;
                    NotifyCanExecuteChanged();

                    await execute(obj);
                    OnExecuted?.Invoke();
                }
                finally
                {
                    isExecuting = false;
                }
            }

            NotifyCanExecuteChanged();
        }

        public void NotifyCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, EventArgs.Empty);
        }

        #region Explicit implementations
        bool ICommand.CanExecute(object parameter)
        {
            return CanExecute(parameter);
        }

        async void ICommand.Execute(object parameter)
        {
            await ExecuteAsync(parameter);
        }
        #endregion
    }
}
