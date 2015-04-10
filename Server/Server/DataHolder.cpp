#include <string>
#include "DataHolder.h"
#include "../DataBase/SQLite/sqlite3.h"

CardsDataMap DataHolder::m_cards;

// Callback for select from cards table
int DataHolder::loadCardsCallback(void* /*data*/, int argc, char** argv, char** /*azColName*/)
{
    if (argc != 6)
        return 1;

    Card card(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    m_cards.insert(std::make_pair(card.GetId(), card));
    return 0;
}

// Loads cards info from database
void DataHolder::loadCards(sqlite3* db)
{
    char* errorMsg;
    std::string sql = "SELECT Id, type, BaseHp, BaseDamage, BaseMana, BaseDefense FROM Cards";
    if (sqlite3_exec(db, sql.c_str(), loadCardsCallback, 0, &errorMsg) != SQLITE_OK)
        printf("Error while loading cards\n");
}

// Loads all data from database
bool DataHolder::LoadData()
{
    sqlite3* db;
    if (sqlite3_open("../DataBase/data.db", &db))
        return false;

    loadCards(db);
    sqlite3_close(db);
    return true;
}
