#pragma once
#include <functional>
#include <string_view>
#include "libpq-fe.h"
#include "PreparedStatement.h"

class DatabaseHandler
{
    private:
        PGconn* m_connection;

    public:
        DatabaseHandler(std::string_view dbName, std::string_view userName, std::string_view password);
        ~DatabaseHandler();
        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const;
};
