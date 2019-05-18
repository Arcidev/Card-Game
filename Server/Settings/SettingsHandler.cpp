#include <fstream>
#include "SettingsHandler.h"
#include "../Server/StaticHelper.h"

SettingsHandler::ConfigMap SettingsHandler::m_configMap =
{
    { "DbName",     [](auto settings, auto value) { settings->m_dbConfig.DbName = value; } },
    { "UserName",   [](auto settings, auto value) { settings->m_dbConfig.UserName = value; } },
    { "Password",   [](auto settings, auto value) { settings->m_dbConfig.Password = value; } }
};

SettingsHandler::SettingsHandler()
{
    std::ifstream settings("CardGame.conf", std::ifstream::in);
    if (!settings.is_open())
        throw std::runtime_error("Config file not found");

    while (settings.good())
    {
        std::string line;
        std::getline(settings, line);

        if (line[0] == '#')
            continue;

        size_t delimiter = line.find(':');
        if (delimiter == std::string::npos || delimiter == line.length() - 1)
            continue;

        std::string config = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);

        StaticHelper::Trim(config);
        StaticHelper::Trim(value);

        ConfigMap::const_iterator iter = m_configMap.find(config);
        if (iter != m_configMap.end())
            iter->second(this, value);
    } 

    settings.close();
}

SettingsHandler const& SettingsHandler::GetInstance()
{
    static SettingsHandler handler;
    return handler;
}
