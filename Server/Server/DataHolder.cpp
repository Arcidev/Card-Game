#include <iostream>
#include "DataHolder.h"
#include "Spells/SpellDefines.h"
#include "Spells/SpellValidator.h"
#include "../DataBase/SQLite/sqlite3.h"

CardsDataMap DataHolder::m_cards;
SpellsDataMap DataHolder::m_spells;

// Callback for selecting spells
int DataHolder::loadSpellsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/)
{
    if (argc != 10)
        return 1;

    uint32_t spellId = atoi(argv[0]);
    uint8_t targetId = atoi(argv[4]);
    if (targetId >= MAX_SPELL_EFFECT_TARGET)
    {
        std::cerr << "Spell id " << spellId << " has invalid target" << std::endl;
        return 0;
    }

    uint8_t spellEffectId = atoi(argv[2]);
    SpellEffectFunc spellEffectFunc = SpellEffect::GetSpellEffectFunc(spellEffectId);
    if (!spellEffectFunc)
    {
        std::cerr << "Spell id " << spellId << " has invalid spell effect" << std::endl;
        return 0;
    }
    
    SpellsDataMap::iterator spellIter = m_spells.find(spellId);
    if (spellIter == m_spells.end())
        spellIter = m_spells.insert(std::make_pair(spellId, Spell(spellId, atoi(argv[5])))).first;

    SpellEffectValues spellVal(spellId, atoi(argv[1]), atoi(argv[3]), targetId, atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));
    spellIter->second.addSpellEffect(std::make_pair(spellEffectFunc, spellVal));

    SpellValidator::ValidateSpellEffect(spellEffectId, spellVal);
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
bool DataHolder::loadSpells(sqlite3* db)
{
    char* errorMsg;
    std::string sql = "SELECT Spells.Id, SpellValues.Id, SpellEffectId, SpellAttributesMask, Target, ManaCost, EffectValue1, EffectValue2, EffectValue3, EffectValue4 FROM (Spells JOIN SpellsSpellValues ON Spells.Id = SpellId) JOIN SpellValues ON SpellValueId = SpellValues.Id";
    if (sqlite3_exec(db, sql.c_str(), loadSpellsCallback, nullptr, &errorMsg) == SQLITE_OK)
        return true;

    std::cerr << "Error while loading spels: " << errorMsg << std::endl;
    sqlite3_free(errorMsg);
    return false;
}

// Loads cards info from database
bool DataHolder::loadCards(sqlite3* db)
{
    char* errorMsg;
    std::string sql = "SELECT Id, type, BaseHp, BaseDamage, BaseMana, BaseDefense, Price, SpellId FROM Cards";
    if (sqlite3_exec(db, sql.c_str(), loadCardsCallback, nullptr, &errorMsg) == SQLITE_OK)
        return true;

    std::cerr << "Error while loading cards: " << errorMsg << std::endl;
    sqlite3_free(errorMsg);
    return false;
}

// Loads all data from database
bool DataHolder::LoadData()
{
    sqlite3* db;
    if (sqlite3_open_v2("DataBase/data.db", &db, SQLITE_OPEN_READONLY, nullptr))
    {
        std::cerr << "Error while opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close_v2(db);
        return false;
    }

    if (!loadSpells(db))
    {
        sqlite3_close_v2(db);
        return false;
    }

    if (!loadCards(db))
    {
        sqlite3_close_v2(db);
        return false;
    }

    sqlite3_close_v2(db);
    return true;
}

Card const* DataHolder::GetCard(uint32_t cardId)
{
    CardsDataMap::const_iterator iter = m_cards.find(cardId);
    return iter != m_cards.end() ? &iter->second : nullptr;
}
