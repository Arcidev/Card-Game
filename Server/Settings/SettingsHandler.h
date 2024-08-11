#pragma once
#include <functional>
#include <map>
#include <string>

struct DatabaseConfig
{
    std::string DbName;
    std::string UserName;
    std::string Password;
};

class SettingsHandler
{
    private:
        typedef std::map<std::string, std::function<void(SettingsHandler*, std::string_view)>> ConfigMap;

        static ConfigMap m_configMap;

        DatabaseConfig m_dbConfig;
        SettingsHandler();

    public:
        static SettingsHandler const& GetInstance();

        DatabaseConfig const& GetDbConfig() const { return m_dbConfig; }
};
