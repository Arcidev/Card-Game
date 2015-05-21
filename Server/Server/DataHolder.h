#pragma once
#include <map>
#include <cstdint>
#include "Spells/Spell.h"
#include "Cards/Card.h"

typedef std::map<uint32_t, Card> CardsDataMap;
typedef std::map<uint32_t, Spell> SpellsDataMap;

struct sqlite3;
class DataHolder
{
    private:
        static SpellsDataMap m_spells;
        static CardsDataMap m_cards;
        
        static void loadSpells(sqlite3* db);
        static void loadCards(sqlite3* db);
        static int loadSpellsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/);
        static int loadCardsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/);

    public:
        static bool LoadData();
        static CardsDataMap const& GetCards() { return m_cards; }
        static Card const* GetCard(uint32_t const& cardId);
};

