#include <iostream>
#include <stdexcept>
#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string_view dbName, std::string_view userName, std::string_view password)
{
    std::string connectionString("dbname=");
    connectionString.append(dbName).append(" user=").append(userName).append(" password=").append(password);
    m_connection = PQconnectdb(connectionString.c_str());

    if (PQstatus(m_connection) != CONNECTION_OK)
        throw std::invalid_argument(PQerrorMessage(m_connection));
}

DatabaseHandler::~DatabaseHandler()
{
    PQfinish(m_connection);
}

void DatabaseHandler::ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const
{
    auto params = statement.GetParameters().empty() ? nullptr : &statement.GetParameters()[0];
    auto sizes = statement.GetSizes().empty() ? nullptr : &statement.GetSizes()[0];
    PGresult* res = PQexecParams(m_connection, statement.GetCommand().data(), (int)statement.GetParameters().size(), nullptr, params, sizes, nullptr, 0);

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
            std::cerr << "Command execution has failed: " << statement.GetCommand() << "Error: " << PQresultErrorMessage(res) << std::endl;
    }
    
    PQclear(res);
}
