using Client.Logic.Data.Cards;
using Client.Logic.Data.Spells;
using System;
using System.Collections.Generic;
using System.Data.SQLite;

namespace Client.Logic.Data
{
    public static class DataHolder
    {
        private static IDictionary<UInt32, SelectableCard> cardsMap;
        private static readonly IDictionary<UInt32, SpellData> spellsDataMap = new Dictionary<UInt32, SpellData>();

        public static IEnumerable<SelectableCard> Cards => cardsMap?.Values;

        // Loads cards from database
        public static void LoadData(IDictionary<UInt32, SelectableCard> cards)
        {
            using (var connection = new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;"))
            {
                connection.Open();
                LoadSpellsData(connection);
                LoadCards(connection, cards);
            }
        }

        // Returns card by id
        public static SelectableCard GetCard(UInt32 id)
        {
            return cardsMap != null && cardsMap.TryGetValue(id, out var card) ? card : null;
        }

        // Returns spells data
        public static SpellData GetSpellData(UInt32 id)
        {
            return spellsDataMap.TryGetValue(id, out var spellData) ? spellData : new SpellData(id, "", "");
        }

        private static void LoadSpellsData(SQLiteConnection connection)
        {
            spellsDataMap.Clear();
            using (var cmd = connection.CreateCommand())
            {
                cmd.CommandText = "SELECT id, name, description FROM Spells";
                using (var result = cmd.ExecuteReader())
                {
                    while (result.Read())
                    {
                        var spellData = new SpellData(Convert.ToUInt32(result["id"]), Convert.ToString(result["name"]), Convert.ToString(result["description"]));
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
                using (var result = cmd.ExecuteReader())
                {
                    while (result.Read())
                    {
                        if (cards.TryGetValue(Convert.ToUInt32(result["id"]), out var card))
                        {
                            card.Name = Convert.ToString(result["name"]);
                            card.ImageUri = $"Assets/{Convert.ToString(result["imagePath"])}";

                            if (card.Spell != null)
                                card.Spell.SpellData = GetSpellData(card.Spell.Id);
                        }
                    }
                }
            }

            cardsMap = cards;
        }
    }
}
