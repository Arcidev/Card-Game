#pragma once
#include <functional>
#include <string_view>
#include "libpq-fe.h"
#include "PreparedStatement.h"

class DatabaseHandler
{
    private:
        PGconn* m_connection;
        void CloseConnection();

    public:
        ~DatabaseHandler();
        bool CreateConnection();
        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback);
};
