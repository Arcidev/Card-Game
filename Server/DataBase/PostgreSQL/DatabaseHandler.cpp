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

void DatabaseHandler::ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback)
{
   PGresult* res = PQexecParams(m_connection, statement.GetCommand().data(), (int)statement.GetParameters().size(), nullptr, &statement.GetParameters()[0], &statement.GetSizes()[0], nullptr, 0);

   ExecStatusType result = PQresultStatus(res);
   switch (PQresultStatus(res))
   {
      case PGRES_COMMAND_OK:
         break;
      case PGRES_TUPLES_OK:
      case PGRES_SINGLE_TUPLE:
         if (callback == nullptr)
            std::cerr << "Missing callback for retrieval function: " << statement.GetCommand() << std::endl;
         else
            callback(res);
         break;
      default:
         std::cerr << "Command execution has failed: " << statement.GetCommand() << std::endl;
   }

    PQclear(res);
}
