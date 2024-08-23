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
        std::list<Db::UserRelation> getUserRelations(PreparedStatement const& stmt) const;
        bool executeContainsCommand(PreparedStatement const& stmt) const;

    public:
        DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password) : dbHandler(dbName, userName, password) { }

        Db::User GetUser(std::string_view email) const;
        Db::User GetUser(uint32_t id) const;
        uint32_t GetUserId(std::string_view name) const;
        void CreateUser(Db::User& user) const;
        void SetUserPassword(uint32_t id, std::string_view passwordSalt, std::string_view passwordHash) const;
        std::tuple<bool, bool> CanCreateUser(std::string_view email, std::string_view userName) const;
        void SetUserActive(uint32_t id, bool active) const;
        void DeleteUser(uint32_t id) const;
        void UpdateUserLastLoginTime(uint32_t id) const;
        std::list<Db::UserRelation> GetUserFriends(uint32_t id) const;
        std::list<Db::UserRelation> GetBlockedUsers(uint32_t id) const;
        std::list<Db::UserRelation> GetFriendRequests(uint32_t id) const;
        bool HasFriendRequest(uint32_t userId, uint32_t receiverId) const;
        bool IsUserBlocked(uint32_t userId, uint32_t blockedId) const;
        void SetFriend(uint32_t userId, uint32_t friendId) const;
        void SetBlockedUser(uint32_t userId, uint32_t friendId) const;
        void SetFriendRequest(uint32_t userId, uint32_t receiverId) const;
        void RemoveFriend(uint32_t userId, uint32_t friendId) const;
        void RemoveBlockedUser(uint32_t userId, uint32_t friendId) const;
        void RemoveFriendRequest(uint32_t userId, uint32_t senderId) const;

        DbAchievementMap GetAchievements() const;
        DbUserCriteriaMap GetUserAchievementProgress(uint32_t userId) const;
        void SetUserAchievementProgress(std::map<std::tuple<uint32_t, uint32_t>, uint32_t> const& userAchievements) const;

        void ExecuteCommand(PreparedStatement const& statement, std::function<void(PGresult const*)> callback) const { dbHandler.ExecuteCommand(statement, callback); }
};
