﻿using Client.Logic.Data.Cards;
using Client.Logic.Data.Spells;
using System;
using System.Collections.Generic;
using System.Data.SQLite;

namespace Client.Logic.Data
{
    public static class DataHolder
    {
        private static IDictionary<UInt32, Card> cardsMap;
        private static IDictionary<UInt32, SpellData> spellsDataMap;

        public static IEnumerable<Card> Cards => cardsMap?.Values;

        // Loads cards from database
        public static void LoadData(IDictionary<UInt32, Card> cards)
        {
            using (SQLiteConnection connection = new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;"))
            {
                connection.Open();
                LoadSpellsData(connection);
                LoadCards(connection, cards);
            }
        }

        // Returns card by id
        public static Card GetCard(UInt32 id)
        {
            return cardsMap.TryGetValue(id, out var card) ? card : null;
        }

        // Returns spells data
        public static SpellData GetSpellData(UInt32 id)
        {
            return spellsDataMap.TryGetValue(id, out var spellData) ? spellData : new SpellData(id, "", "");
        }

        private static void LoadSpellsData(SQLiteConnection connection)
        {
            spellsDataMap = new Dictionary<UInt32, SpellData>();
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
        private static void LoadCards(SQLiteConnection connection, IDictionary<UInt32, Card> cards)
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
