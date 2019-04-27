#pragma once
#include <string_view>
#include <vector>

class PreparedStatement
{
   private:
      std::string_view m_command;
      std::vector<char const*> m_params;
      std::vector<int> m_sizes;

   public:
      PreparedStatement(std::string_view command) : m_command(command) { }

      void AddParameter(std::string_view parameter) { m_params.push_back(parameter.data()); m_sizes.push_back((int)parameter.size()); }
      std::vector<char const*> const& GetParameters() const { return m_params; }
      std::vector<int> const& GetSizes() const { return m_sizes; }

      std::string_view GetCommand() const { return m_command; }
};
