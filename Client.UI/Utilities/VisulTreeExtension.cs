using System.Windows;
using System.Windows.Media;

namespace Client.UI.Utilities
{
    public static class VisulTreeExtension
    {
        public static T FindParent<T>(this DependencyObject obj) where T : DependencyObject
        {
            if (obj == null)
                return null;

            obj = VisualTreeHelper.GetParent(obj);
            if (obj is T)
                return obj as T;

            return FindParent<T>(obj);
        }
    }
}
