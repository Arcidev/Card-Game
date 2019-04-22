#pragma once
#include <functional>
#include <string_view>
#include "libpq-fe.h"

class DatabaseHandler
{
    private:
        PGconn* m_connection;
        void CloseConnection();

    public:
        ~DatabaseHandler();
        bool CreateConnection();
        bool ExecuteCommand(std::string_view command);
        void FetchCommand(std::string_view command, std::function<void()> callback);
};
