using System.Windows;
using System.Windows.Media;

namespace Client.UI.Utilities
{
    public static class VisulTreeExtension
    {
        public static T FindParent<T>(this DependencyObject obj) where T : DependencyObject
        {
            while (obj != null && obj is not T)
                obj = VisualTreeHelper.GetParent(obj);

            return obj as T;
        }
    }
}
