#include <iostream>
#include <sstream>
#include <stdexcept>
#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string_view dbName, std::string_view userName, std::string_view password)
{
    std::ostringstream connectionString;
    connectionString << "dbname=" << dbName << " username=" << userName << " password=" << password;
    m_connection = PQconnectdb(connectionString.str().c_str());

    if (PQstatus(m_connection) != CONNECTION_OK)
        throw std::invalid_argument("Invalid connection arguments");
}

DatabaseHandler::~DatabaseHandler()
{
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
