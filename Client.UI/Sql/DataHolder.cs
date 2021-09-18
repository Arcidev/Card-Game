using Client.Logic.Data;
using Client.Logic.Data.Achievements;
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
        private readonly Dictionary<UInt32, AchievementInfo> achievements = new();
        private readonly Dictionary<UInt32, string> criteriaDescriptions = new();

        public IEnumerable<SelectableCard> Cards => cards?.Values;

        public DataHolder()
        {
            using var connection = GetConnection();
            connection.Open();
            LoadAchievements(connection);
            LoadCriterias(connection);
        }

        // Loads data from database
        public void LoadData(Dictionary<UInt32, SelectableCard> cards)
        {
            using var connection = GetConnection();
            connection.Open();
            LoadSpellsData(connection);
            LoadCards(connection, cards);
        }

        public SelectableCard GetCard(UInt32 id) => cards != null && cards.TryGetValue(id, out var card) ? card : null;

        public SpellData GetSpellData(UInt32 id) => spellsData.TryGetValue(id, out var spellData) ? spellData : new SpellData(id, string.Empty, string.Empty, string.Empty, string.Empty);

        public AchievementInfo GetAchievementInfo(UInt32 id) => achievements.TryGetValue(id, out var name) ? name : null;

        public string GetCriteriaDescription(UInt32 id) => criteriaDescriptions.TryGetValue(id, out var description) ? description : string.Empty;

        private SQLiteConnection GetConnection() => new SQLiteConnection("Data Source=Assets/Data/data.db;Version=3;New=False;Compress=True;");

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

        private void LoadAchievements(SQLiteConnection connection)
        {
            achievements.Clear();
            using var cmd = connection.CreateCommand();
            cmd.CommandText = "SELECT id, name, image FROM Achievements";
            using var result = cmd.ExecuteReader();

            while (result.Read())
            {
                var name = Convert.ToString(result["name"]);
                var image = Convert.ToString(result["image"]);
                achievements.Add(Convert.ToUInt32(result["id"]), new AchievementInfo(name, image));
            }
        }

        private void LoadCriterias(SQLiteConnection connection)
        {
            criteriaDescriptions.Clear();
            using var cmd = connection.CreateCommand();
            cmd.CommandText = "SELECT id, description FROM Criterias";
            using var result = cmd.ExecuteReader();

            while (result.Read())
                criteriaDescriptions.Add(Convert.ToUInt32(result["id"]), Convert.ToString(result["description"]));
        }
    }
}
