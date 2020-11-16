using Client.Logic.Data;
using Client.Logic.Data.Cards;
using Client.Logic.Data.Spells;
using System;
using System.Collections.Generic;
using System.Data.SQLite;

namespace Client.UI.Sql
{
    public class DataHolder : IDataHolder
    {
        private Dictionary<UInt32, SelectableCard> cards;
        private readonly Dictionary<UInt32, SpellData> spellsData = new();

        public IEnumerable<SelectableCard> Cards => cards?.Values;

        // Loads cards from database
        public void LoadData(Dictionary<UInt32, SelectableCard> cards)
        {
            using var connection = new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;");
            connection.Open();
            LoadSpellsData(connection);
            LoadCards(connection, cards);
        }

        // Returns card by id
        public SelectableCard GetCard(UInt32 id) => cards != null && cards.TryGetValue(id, out var card) ? card : null;

        // Returns spells data
        public SpellData GetSpellData(UInt32 id) => spellsData.TryGetValue(id, out var spellData) ? spellData : new SpellData(id, string.Empty, string.Empty, string.Empty, string.Empty);

        private void LoadSpellsData(SQLiteConnection connection)
        {
            spellsData.Clear();
            using var cmd = connection.CreateCommand();
            cmd.CommandText = "SELECT id, name, description, text, imagePath FROM Spells LEFT JOIN SpellAuras on Id = SpellId";
            using var result = cmd.ExecuteReader();

            while (result.Read())
            {
                var spellData = new SpellData(Convert.ToUInt32(result["id"]), Convert.ToString(result["name"]), Convert.ToString(result["description"]), Convert.ToString(result["text"]), Convert.ToString(result["imagePath"]));
                spellsData.Add(spellData.SpellId, spellData);
            }
        }

        // Loads cards
        private void LoadCards(SQLiteConnection connection, Dictionary<UInt32, SelectableCard> cards)
        {
            using var cmd = connection.CreateCommand();
            cmd.CommandText = "SELECT id, name, imagePath FROM Cards";
            using var result = cmd.ExecuteReader();

            while (result.Read())
            {
                if (cards.TryGetValue(Convert.ToUInt32(result["id"]), out var card))
                {
                    card.Name = Convert.ToString(result["name"]);
                    card.ImageUri = Convert.ToString(result["imagePath"]);

                    if (card.Spell != null)
                        card.Spell.SpellData = GetSpellData(card.Spell.Id);
                }
            }

            this.cards = cards;
        }
    }
}
