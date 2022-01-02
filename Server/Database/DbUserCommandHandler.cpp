#include <cstring>
#include <string>
#include "DbCommandHandler.h"

Db::User DbCommandHandler::executeGetUserCommand(PreparedStatement const& stmt) const
{
    Db::User user;
    dbHandler.ExecuteCommand(stmt, [&user](auto res)
    {
        if (PQntuples(res) < 1)
            return;

        user.Id = (uint32_t)strtoul(PQgetvalue(res, 0, 0), nullptr, 0);
        user.Email = PQgetvalue(res, 0, 1);
        user.UserName = PQgetvalue(res, 0, 2);
        user.PasswordSalt = PQgetvalue(res, 0, 3);
        user.PasswordHash = PQgetvalue(res, 0, 4);
        user.UserRole = (uint8_t)strtoul(PQgetvalue(res, 0, 5), nullptr, 0);
    });

    return user;
}

std::list<Db::UserRelation> DbCommandHandler::getUserRelations(PreparedStatement const& stmt) const
{
    std::list<Db::UserRelation> users;
    dbHandler.ExecuteCommand(stmt, [&users](auto res)
    {
        int count = PQntuples(res);
        if (count < 1)
            return;

        for (int i = 0; i < count; i++)
        {
            Db::UserRelation user =
            {
                (uint32_t)strtoul(PQgetvalue(res, i, 0), nullptr, 0), // User Id
                PQgetvalue(res, i, 1), // User Name
            };

            users.push_back(user);
        }
    });

    return users;
}

bool DbCommandHandler::executeContainsCommand(PreparedStatement const& stmt) const
{
    bool result = false;
    dbHandler.ExecuteCommand(stmt, [&result](auto res)
    {
        result = PQntuples(res) > 0;
    });

    return result;
}

Db::User DbCommandHandler::GetUser(std::string_view email) const
{
    PreparedStatement stmt("SELECT id, email, user_name, password_salt, password_hash, user_role FROM users WHERE email = $1 AND is_active;");
    stmt.AddParameter(email);

    return executeGetUserCommand(stmt);
}

Db::User DbCommandHandler::GetUser(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("SELECT id, email, user_name, password_salt, password_hash, user_role FROM users WHERE id = $1 AND is_active;");
    stmt.AddParameter(idStr);

    return executeGetUserCommand(stmt);
}

uint32_t DbCommandHandler::GetUserId(std::string_view name) const
{
    PreparedStatement stmt("SELECT id FROM users WHERE user_name = $1 AND is_active;");
    stmt.AddParameter(name);

    uint32_t id = 0;
    dbHandler.ExecuteCommand(stmt, [&id](auto res)
    {
        if (PQntuples(res) < 1)
            return;

        id = (uint32_t)strtoul(PQgetvalue(res, 0, 0), nullptr, 0);
    });

    return id;
}

void DbCommandHandler::CreateUser(Db::User& user) const
{
    std::string userRoleStr = std::to_string(user.UserRole);
    PreparedStatement stmt("INSERT INTO users (email, user_name, password_salt, password_hash, user_role, is_active) VALUES ($1, $2, $3, $4, $5, true) RETURNING id;");
    stmt.AddParameter(user.Email);
    stmt.AddParameter(user.UserName);
    stmt.AddParameter(user.PasswordSalt);
    stmt.AddParameter(user.PasswordHash);
    stmt.AddParameter(userRoleStr);

    dbHandler.ExecuteCommand(stmt, [&user](auto res) { user.Id = (uint32_t)strtoul(PQgetvalue(res, 0, 0), nullptr, 0); });
}

void DbCommandHandler::SetUserPassword(uint32_t id, std::string_view passwordSalt, std::string_view passwordHash) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("UPDATE users SET password_salt = $1, password_hash = $2 WHERE id = $3;");
    stmt.AddParameter(passwordSalt);
    stmt.AddParameter(passwordHash);
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

std::tuple<bool, bool> DbCommandHandler::CanCreateUser(std::string_view email, std::string_view userName) const
{
    PreparedStatement stmt("SELECT email = $1, user_name = $2 FROM users WHERE email = $1 OR user_name = $2;");
    stmt.AddParameter(email);
    stmt.AddParameter(userName);

    bool emailInUse = false;
    bool userNameInUse = false;
    dbHandler.ExecuteCommand(stmt, [&emailInUse, &userNameInUse](auto res)
    {
        if (!PQntuples(res))
            return;

        emailInUse = !strcmp(PQgetvalue(res, 0, 0), "t");
        userNameInUse = !strcmp(PQgetvalue(res, 0, 1), "t");
    });

    return { emailInUse, userNameInUse };
}

void DbCommandHandler::SetUserActive(uint32_t id, bool active) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("UPDATE users SET is_active = $1 WHERE id = $2;");
    stmt.AddParameter(active ? "true" : "false");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::DeleteUser(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("DELETE FROM users WHERE id = $1;");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::UpdateUserLastLoginTime(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("UPDATE users SET last_login_on = CURRENT_TIMESTAMP WHERE id = $1;");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

std::list<Db::UserRelation> DbCommandHandler::GetUserFriends(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("SELECT friend_id, user_name FROM friends INNER JOIN users ON friend_id = users.id WHERE user_id = $1;");
    stmt.AddParameter(idStr);

    return getUserRelations(stmt);
}

std::list<Db::UserRelation> DbCommandHandler::GetBlockedUsers(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("SELECT blocked_id, user_name FROM blocked_users INNER JOIN users ON blocked_id = users.id WHERE user_id = $1;");
    stmt.AddParameter(idStr);

    return getUserRelations(stmt);
}

std::list<Db::UserRelation> DbCommandHandler::GetFriendRequests(uint32_t id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("SELECT sender_id, user_name FROM friend_requests INNER JOIN users ON sender_id = users.id WHERE receiver_id = $1;");
    stmt.AddParameter(idStr);

    return getUserRelations(stmt);
}

bool DbCommandHandler::IsUserBlocked(uint32_t userId, uint32_t blockedId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string blockedIdStr = std::to_string(blockedId);
    PreparedStatement stmt("SELECT 1 FROM blocked_users WHERE user_id = $1 AND blocked_id = $2;");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(blockedIdStr);

    return executeContainsCommand(stmt);
}

bool DbCommandHandler::HasFriendRequest(uint32_t userId, uint32_t receiverId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string receiverIdStr = std::to_string(receiverId);
    PreparedStatement stmt("SELECT 1 FROM friend_requests WHERE sender_id = $1 AND receiver_id = $2;");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(receiverIdStr);

    return executeContainsCommand(stmt);
}

void DbCommandHandler::SetFriend(uint32_t userId, uint32_t friendId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string friendIdStr = std::to_string(friendId);

    PreparedStatement stmt("INSERT INTO friends (user_id, friend_id) VALUES ($1, $2) ON CONFLICT DO NOTHING;");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(friendIdStr);
    dbHandler.ExecuteCommand(stmt, nullptr);

    stmt = PreparedStatement("INSERT INTO friends (user_id, friend_id) VALUES ($1, $2) ON CONFLICT DO NOTHING;");
    stmt.AddParameter(friendIdStr);
    stmt.AddParameter(userIdStr);
    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::SetBlockedUser(uint32_t userId, uint32_t blockedId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string blockedIdStr = std::to_string(blockedId);

    PreparedStatement stmt("INSERT INTO blocked_users (user_id, blocked_id) VALUES ($1, $2) ON CONFLICT DO NOTHING;");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(blockedIdStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::SetFriendRequest(uint32_t userId, uint32_t receiverId) const
{
    std::string senderIdStr = std::to_string(userId);
    std::string receiverIdStr = std::to_string(receiverId);

    PreparedStatement stmt("INSERT INTO friend_requests (sender_id, receiver_id) VALUES ($1, $2) ON CONFLICT DO NOTHING;");
    stmt.AddParameter(senderIdStr);
    stmt.AddParameter(receiverIdStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::RemoveFriend(uint32_t userId, uint32_t friendId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string friendIdStr = std::to_string(friendId);
    PreparedStatement stmt("DELETE FROM friends WHERE (user_id = $1 AND friend_id = $2) OR (user_id = $2 AND friend_id = $1);");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(friendIdStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::RemoveBlockedUser(uint32_t userId, uint32_t blockedId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string blockedIdStr = std::to_string(blockedId);
    PreparedStatement stmt("DELETE FROM blocked_users WHERE user_id = $1 AND blocked_id = $2;");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(blockedIdStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::RemoveFriendRequest(uint32_t userId, uint32_t senderId) const
{
    std::string userIdStr = std::to_string(userId);
    std::string senderIdStr = std::to_string(senderId);
    PreparedStatement stmt("DELETE FROM friend_requests WHERE (receiver_id = $1 AND sender_id = $2) OR (receiver_id = $2 AND sender_id = $1);");
    stmt.AddParameter(userIdStr);
    stmt.AddParameter(senderIdStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}
