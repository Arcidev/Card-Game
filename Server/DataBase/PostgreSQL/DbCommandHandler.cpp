#include <string>
#include "DbCommandHandler.h"

DbCommandHandler::DbCommandHandler(std::string_view dbName, std::string_view userName, std::string_view password) : dbHandler(dbName, userName, password)
{

}

User DbCommandHandler::GetUser(std::string_view email)
{
    PreparedStatement stmt("SELECT id, email, password_salt, password_hash, user_role FROM users WHERE email = $1;");
    stmt.AddParameter(email);

    User user;
    dbHandler.ExecuteCommand(stmt, [&user](auto res)
    {
        for (int i = 0; i < PQntuples(res); i++)
        {
            user.Id = *(uint32_t *)PQgetvalue(res, i, 0);
            user.Email = PQgetvalue(res, i, 1);
            user.PasswordSalt = PQgetvalue(res, i, 2);
            user.PasswordHash = PQgetvalue(res, i, 3);
            user.UserRole = (uint8_t)*(uint16_t *)PQgetvalue(res, i, 0);
        }
    });

    return user;
}

void DbCommandHandler::CreateUser(User& user)
{
    PreparedStatement stmt("INSERT INTO users (email, password_salt, password_hash, user_role) VALUES ($1, $2, $3, $4) RETURNING Id;");
    stmt.AddParameter(user.Email);
    stmt.AddParameter(user.PasswordSalt);
    stmt.AddParameter(user.PasswordHash);
    stmt.AddParameter(std::to_string(user.UserRole));

    dbHandler.ExecuteCommand(stmt, [&user](auto res) { user.Id = *(uint32_t *)PQgetvalue(res, 0, 0); });
}
