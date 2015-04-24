#pragma once
#include <map>
#include <cstdint>
#include "Card/Card.h"

typedef std::map<uint32_t, Card> CardsDataMap;

struct sqlite3;
class DataHolder
{
    private:
        static CardsDataMap m_cards;

        static void loadCards(sqlite3* db);
        static int loadCardsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/);

    public:
        static bool LoadData();
        static CardsDataMap const& GetCards() { return m_cards; }
};

