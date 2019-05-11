#include "SettingsHandler.h"

SettingsHandler::SettingsHandler()
{
    
}

SettingsHandler const& SettingsHandler::GetInstance()
{
    static SettingsHandler handler;
    return handler;
}
