#pragma once
#include "DatabaseHandler.h"
#include "DbEntities.h"

class DbCommandHandler
{
    private:
        DatabaseHandler dbHandler;

    public:
        DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password);

        User GetUser(std::string_view email);
        void CreateUser(User& user);
        void SetUserActive(int id, bool active);
        void DeleteUser(int id);

        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const { dbHandler.ExecuteCommand(statement, callback); }
};
