using Client.Enums;
using System.Collections.Generic;
using System.IO;
using System.Windows.Input;

namespace Client.Misc
{
    public static class CardAttackCursors
    {
        private static readonly Cursor meleeCursor = new Cursor(Path.Combine(Directory.GetCurrentDirectory(), "Assets/Cursors/Cursor_Sword.cur"));
        private static readonly Cursor arrowCursor = new Cursor(Path.Combine(Directory.GetCurrentDirectory(), "Assets/Cursors/Cursor_Arrow.cur"));
        private static readonly Cursor staffCursor = new Cursor(Path.Combine(Directory.GetCurrentDirectory(), "Assets/Cursors/Cursor_Staff.cur"));

        private static readonly Dictionary<CardAttackCursorTypes, Cursor> cursors = new Dictionary<CardAttackCursorTypes, Cursor>()
        {
            { CardAttackCursorTypes.Sword, meleeCursor },
            { CardAttackCursorTypes.Arrow, arrowCursor },
            { CardAttackCursorTypes.Staff, staffCursor }
        };

        public static Cursor GetCursor(CardAttackCursorTypes type)
        {
            return cursors[type];
        }
    }
}
