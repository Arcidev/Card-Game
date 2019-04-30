#include <string>
#include "DbCommandHandler.h"

DbCommandHandler::DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password) : dbHandler(dbName, userName, password)
{

}

User DbCommandHandler::GetUser(std::string_view email)
{
    PreparedStatement stmt("SELECT id, email, user_name, password_salt, password_hash, user_role FROM users WHERE email = $1 AND is_active;");
    stmt.AddParameter(email);

    User user;
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

void DbCommandHandler::CreateUser(User& user)
{
    PreparedStatement stmt("INSERT INTO users (email, user_name, password_salt, password_hash, user_role, is_active) VALUES ($1, $2, $3, $4, $5, true) RETURNING id;");
    stmt.AddParameter(user.Email);
    stmt.AddParameter(user.UserName);
    stmt.AddParameter(user.PasswordSalt);
    stmt.AddParameter(user.PasswordHash);
    stmt.AddParameter(std::to_string(user.UserRole));

    dbHandler.ExecuteCommand(stmt, [&user](auto res) { user.Id = (uint32_t)strtoul(PQgetvalue(res, 0, 0), nullptr, 0); });
}

void DbCommandHandler::SetUserActive(int id, bool active)
{
    PreparedStatement stmt("UPDATE users SET is_active = $1 WHERE id = $2;");
    stmt.AddParameter(active ? "true" : "false");
    stmt.AddParameter(std::to_string(id));

    dbHandler.ExecuteCommand(stmt, nullptr);
}

void DbCommandHandler::DeleteUser(int id)
{
    PreparedStatement stmt("DELETE FROM users WHERE id = $1;");
    stmt.AddParameter(std::to_string(id));

    dbHandler.ExecuteCommand(stmt, nullptr);
}
