#include <iostream>
#include <string>
#include "DataHolder.h"
#include "../DataBase/SQLite/sqlite3.h"

CardsDataMap DataHolder::m_cards;
SpellsDataMap DataHolder::m_spells;

// Callback for selecting spells
int DataHolder::loadSpellsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/)
{
    if (argc != 9)
        return 1;

    uint32_t spellId = atoi(argv[0]);
    uint8_t targetId = atoi(argv[3]);
    if (targetId >= MAX_SPELL_EFFECT_TARGET)
    {
        std::cerr << "Spell id " << spellId << " has invalid target" << std::endl;
        return 0;
    }

    SpellEffectFunc spellEffectFunc = SpellEffect::GetSpellEffectFunc(atoi(argv[1]));
    if (!spellEffectFunc)
    {
        std::cerr << "Spell id " << spellId << " has invalid spell effect" << std::endl;
        return 0;
    }
    
    SpellsDataMap::iterator spellIter = m_spells.find(spellId);
    if (spellIter == m_spells.end())
        spellIter = m_spells.insert(std::make_pair(spellId, Spell(spellId, atoi(argv[4])))).first;

    SpellEffectValues spellVal(spellId, atoi(argv[2]), targetId, atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
    spellIter->second.addSpellEffect(std::make_pair(spellEffectFunc, spellVal));
    return 0;
}

// Callback for selecting cards
int DataHolder::loadCardsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/)
{
    if (argc != 8)
        return 1;

    Spell const* spell = nullptr;
    if (argv[7])
    {
        SpellsDataMap::const_iterator spellIter = m_spells.find(atoi(argv[7]));
        if (spellIter != m_spells.end())
            spell = &spellIter->second;
    }

    Card card(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), spell);
    m_cards.insert(std::make_pair(card.GetId(), card));
    return 0;
}

// Loads spells from database
void DataHolder::loadSpells(sqlite3* db)
{
    char* errorMsg;
    std::string sql = "SELECT Spells.Id, SpellEffectId, SpellAttributesMask, Target, ManaCost, EffectValue1, EffectValue2, EffectValue3, EffectValue4 FROM (Spells JOIN SpellsSpellValues ON Spells.Id = SpellId) JOIN SpellValues ON SpellValueId = SpellValues.Id";
    if (sqlite3_exec(db, sql.c_str(), loadSpellsCallback, nullptr, &errorMsg) != SQLITE_OK)
        std::cerr << "Error while loading spels: " << errorMsg << std::endl;
}

// Loads cards info from database
void DataHolder::loadCards(sqlite3* db)
{
    char* errorMsg;
    std::string sql = "SELECT Id, type, BaseHp, BaseDamage, BaseMana, BaseDefense, Price, SpellId FROM Cards";
    if (sqlite3_exec(db, sql.c_str(), loadCardsCallback, nullptr, &errorMsg) != SQLITE_OK)
        std::cerr << "Error while loading cards: " << errorMsg << std::endl;
}

// Loads all data from database
bool DataHolder::LoadData()
{
    sqlite3* db;
    if (sqlite3_open("../DataBase/data.db", &db))
        return false;

    loadSpells(db);
    loadCards(db);
    sqlite3_close(db);
    return true;
}

Card const* DataHolder::GetCard(uint32_t const& cardId)
{
    CardsDataMap::const_iterator iter = m_cards.find(cardId);
    return iter != m_cards.end() ? &iter->second : nullptr;
}
