#pragma once
#include <cstdint>
#include <map>
#include "Spells/Spell.h"
#include "Cards/Card.h"
#include "../Database/DatabaseInstance.h"

typedef std::map<uint32_t, Card> CardsDataMap;
typedef std::map<uint32_t, Spell> SpellsDataMap;

class DataHolder
{
    private:
        static SpellsDataMap m_spells;
        static CardsDataMap m_cards;
        
        static void loadSpells();
        static void loadCards();
        static void loadSpellsCallback(PGresult const* result);
        static void loadCardsCallback(PGresult const* result);

    public:
        static void LoadData();
        static CardsDataMap const& GetCards() { return m_cards; }
        static Card const* GetCard(uint32_t cardId);
};

