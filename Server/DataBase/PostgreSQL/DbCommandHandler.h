#pragma once
#include "DatabaseHandler.h"
#include "DbEntities.h"

class DbCommandHandler
{
    private:
        DatabaseHandler dbHandler;

    public:
        DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password);

        User GetUser(std::string_view email) const;
        void CreateUser(User& user) const;
        void SetUserActive(int id, bool active) const;
        void DeleteUser(int id) const;

        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const { dbHandler.ExecuteCommand(statement, callback); }
};
