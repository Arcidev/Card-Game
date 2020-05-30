using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Client.UI.ViewModels
{
    /// <summary>
    /// Implementation of <see cref="INotifyPropertyChanged"/>
    /// </summary>
    public abstract class NotifyPropertyViewModel : INotifyPropertyChanged
    {
        /// <summary>
        /// Event invoked when property has been changed
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Raises PropertyChanged event
        /// </summary>
        /// <param name="propertyName">Property name on which the change occured</param>
        protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            if (propertyName == null)
                return;

            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
