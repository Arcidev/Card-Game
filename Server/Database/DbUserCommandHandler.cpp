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

void DbCommandHandler::SetUserActive(int id, bool active) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("UPDATE users SET is_active = $1 WHERE id = $2;");
    stmt.AddParameter(active ? "true" : "false");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::DeleteUser(int id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("DELETE FROM users WHERE id = $1;");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::UpdateUserLastLoginTime(int id) const
{
    std::string idStr = std::to_string(id);
    PreparedStatement stmt("UPDATE users SET last_login_on = CURRENT_TIMESTAMP WHERE id = $1;");
    stmt.AddParameter(idStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}
