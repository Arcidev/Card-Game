using Client.Game;
using System;
using System.Collections.Generic;
using System.Data.SQLite;

namespace Client.Data
{
    public static class DataHolder
    {
        private static IDictionary<UInt32, SelectableCard> cardsMap;
        private static IDictionary<UInt32, SpellData> spellsDataMap;

        public static IEnumerable<SelectableCard> Cards { get { return cardsMap != null ? cardsMap.Values : null; } }
        
        // Loads cards from database
        public static void LoadData(IDictionary<UInt32, SelectableCard> cards)
        {
            using (SQLiteConnection connection = new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;"))
            {
                connection.Open();
                LoadSpellsData(connection);
                LoadCards(connection, cards);
            }
        }

        // Returns card by id
        public static SelectableCard GetCard(UInt32 id)
        {
            SelectableCard card;
            if (cardsMap.TryGetValue(id, out card))
                return card;

            return null;
        }

        // Returns spells data
        public static SpellData GetSpellData(UInt32 id)
        {
            SpellData spellData;
            if (spellsDataMap.TryGetValue(id, out spellData))
                return spellData;

            return new SpellData(id, "", "", null);
        }

        // Unloads data from memory
        public static void UnloadCards() { cardsMap = null; }

        private static void LoadSpellsData(SQLiteConnection connection)
        {
            spellsDataMap = new Dictionary<UInt32, SpellData>();
            using (var cmd = connection.CreateCommand())
            {
                cmd.CommandText = "SELECT id, name, description, spellEffectPath FROM Spells";
                using (SQLiteDataReader result = cmd.ExecuteReader())
                {
                    while (result.Read())
                    {
                        SpellData spellData = new SpellData(Convert.ToUInt32(result["id"]), Convert.ToString(result["name"]), Convert.ToString(result["description"]), Convert.ToString(result["spellEffectPath"]));
                        spellsDataMap.Add(spellData.SpellId, spellData);
                    }
                }
            }
        }

        // Loads cards
        private static void LoadCards(SQLiteConnection connection, IDictionary<UInt32, SelectableCard> cards)
        {
            using (var cmd = connection.CreateCommand())
            {
                cmd.CommandText = "SELECT id, name, imagePath FROM Cards";
                using (SQLiteDataReader result = cmd.ExecuteReader())
                {
                    while (result.Read())
                    {
                        SelectableCard card;
                        if (cards.TryGetValue(Convert.ToUInt32(result["id"]), out card))
                        {
                            card.Name = Convert.ToString(result["name"]);
                            card.ImageUri = string.Format("{0}/{1}", "Assets", Convert.ToString(result["imagePath"]));
                        }

                        if (card.Spell != null)
                            card.Spell.SpellData = GetSpellData(card.Spell.Id);
                    }
                }
            }

            cardsMap = cards;
        }
    }
}
