#pragma once
#include <tuple>
#include "DatabaseHandler.h"
#include "DbEntities.h"

class DbCommandHandler
{
    private:
        DatabaseHandler dbHandler;
        User executeGetUserCommand(PreparedStatement const& stmt) const;

    public:
        DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password);

        User GetUser(std::string_view email) const;
        User GetUser(uint32_t id) const;
        void CreateUser(User& user) const;
        void SetUserPassword(uint32_t id, std::string_view passwordSalt, std::string_view passwordHash) const;
        std::tuple<bool, bool> CanCreateUser(std::string_view email, std::string_view userName) const;
        void SetUserActive(int id, bool active) const;
        void DeleteUser(int id) const;

        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const { dbHandler.ExecuteCommand(statement, callback); }
};
