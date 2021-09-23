#pragma once
#include <map>
#include <tuple>
#include "DatabaseHandler.h"
#include "DbEntities.h"

typedef std::map<uint32_t, Db::Achievement> DbAchievementMap;
typedef std::map<uint32_t, Db::UserAchievementCriteria> DbUserCriteriaMap;

class DbCommandHandler
{
    private:
        DatabaseHandler dbHandler;
        Db::User executeGetUserCommand(PreparedStatement const& stmt) const;

    public:
        DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password) : dbHandler(dbName, userName, password) { }

        Db::User GetUser(std::string_view email) const;
        Db::User GetUser(uint32_t id) const;
        void CreateUser(Db::User& user) const;
        void SetUserPassword(uint32_t id, std::string_view passwordSalt, std::string_view passwordHash) const;
        std::tuple<bool, bool> CanCreateUser(std::string_view email, std::string_view userName) const;
        void SetUserActive(int id, bool active) const;
        void DeleteUser(int id) const;
        void UpdateUserLastLoginTime(int id) const;

        DbAchievementMap GetAchievements() const;
        DbUserCriteriaMap GetUserAchievementProgress(uint32_t userId) const;
        void SetUserAchievementProgress(uint32_t userId, uint32_t criteriaId, uint32_t progress) const;

        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const { dbHandler.ExecuteCommand(statement, callback); }
};
