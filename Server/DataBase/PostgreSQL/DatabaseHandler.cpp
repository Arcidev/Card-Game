#include <iostream>
#include "DatabaseHandler.h"

DatabaseHandler::~DatabaseHandler()
{
    CloseConnection();
}

bool DatabaseHandler::CreateConnection()
{
    m_connection = PQconnectdb("dbname = CardGame");
    if (PQstatus(m_connection) == CONNECTION_OK)
        return true;

    std::cerr << "Connection to database failed: " << PQerrorMessage(m_connection);
    m_connection = nullptr;
    return false;
}

void DatabaseHandler::CloseConnection()
{
    if (!m_connection)
        return;

    PQfinish(m_connection);
}

bool DatabaseHandler::ExecuteCommand(std::string_view command)
{
    PGresult* res = PQexec(m_connection, command.data());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("Drop employee table failed.");
        PQclear(res);
        return false;
    }

    printf("Drop employee table - OK\n");

    PQclear(res);
    return true;
}

void DatabaseHandler::FetchCommand(std::string_view command, std::function<void()> callback)
{

}