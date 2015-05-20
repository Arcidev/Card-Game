using Client.Game;
using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace Client.Data
{
    public static class DataHolder
    {
        private static IDictionary<UInt32, SelectableCard> cardsMap;

        public static IEnumerable<SelectableCard> Cards { get { return cardsMap != null ? cardsMap.Values : null; } }
        
        // Loads cards from database
        public static void LoadData(IDictionary<UInt32, SelectableCard> cards)
        {
            using (SQLiteConnection connection = new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;"))
            {
                connection.Open();
                using (var cmd = connection.CreateCommand())
                {
                    cmd.CommandText = "SELECT id, name, imagePath FROM cards";
                    var result = cmd.ExecuteReader();

                    while (result.Read())
                    {
                        SelectableCard card;
                        if (cards.TryGetValue(Convert.ToUInt32(result["id"]), out card))
                        {
                            card.Name = Convert.ToString(result["name"]);
                            card.ImageUri = string.Format("{0}/{1}", "Assets", Convert.ToString(result["imagePath"]));
                        }

                    }
                }
            }

            cardsMap = cards;
        }

        public static SelectableCard GetCard(UInt32 id)
        {
            SelectableCard card;
            if (cardsMap.TryGetValue(id, out card))
                return card;

            return null;
        }

        // Unloads data from memory
        public static void UnloadData() { cardsMap = null; }
    }
}
