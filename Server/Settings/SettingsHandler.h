#pragma once
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
        DatabaseConfig m_dbConfig;
        SettingsHandler();

    public:
        static SettingsHandler const& GetInstance();

        DatabaseConfig const& GetDbConfig() const { return m_dbConfig; }
};
