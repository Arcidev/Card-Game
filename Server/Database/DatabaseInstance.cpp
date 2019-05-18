#include "DatabaseInstance.h"
#include "../Settings/SettingsHandler.h"

DbCommandHandler const& DatabaseInstance::GetDbCommandHandler()
{
    DatabaseConfig dbConfig = SettingsHandler::GetInstance().GetDbConfig();
    static DbCommandHandler commandHandler(dbConfig.DbName, dbConfig.UserName, dbConfig.Password);
    return commandHandler;
}
