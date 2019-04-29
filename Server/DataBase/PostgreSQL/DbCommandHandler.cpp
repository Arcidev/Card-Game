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

        user.Id = *(uint32_t *)PQgetvalue(res, 0, 0);
        user.Email = PQgetvalue(res, 0, 1);
        user.UserName = PQgetvalue(res, 0, 2);
        user.PasswordSalt = PQgetvalue(res, 0, 3);
        user.PasswordHash = PQgetvalue(res, 0, 4);
        user.UserRole = (uint8_t)*(uint16_t *)PQgetvalue(res, 0, 5);
    });

    return user;
}

void DbCommandHandler::CreateUser(User& user)
{
    PreparedStatement stmt("INSERT INTO users (email, user_name, password_salt, password_hash, user_role, is_active) VALUES ($1, $2, $3, $4, S5, true) RETURNING id;");
    stmt.AddParameter(user.Email);
    stmt.AddParameter(user.UserName);
    stmt.AddParameter(user.PasswordSalt);
    stmt.AddParameter(user.PasswordHash);
    stmt.AddParameter(std::to_string(user.UserRole));

    dbHandler.ExecuteCommand(stmt, [&user](auto res) { user.Id = *(uint32_t *)PQgetvalue(res, 0, 0); });
}
