using CardGameWPF.Game;
using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace CardGameWPF.Data
{
    public static class DataHolder
    {
        public static IEnumerable<Card> Cards { get; private set; }

        // Loads cards from database
        public static void LoadData(IEnumerable<Card> cards)
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
                        var card = cards.FirstOrDefault(x => x.Id == Convert.ToInt32(result["id"]));
                        if (card != null)
                        {
                            card.Name = Convert.ToString(result["name"]);
                            card.ImageUri = string.Format("{0}/{1}", "Assets", Convert.ToString(result["imagePath"]));
                        }

                    }
                }
            }

            Cards = cards;
        }

        // Unloads data from memory
        public static void UnloadData() { Cards = null; }
    }
}
