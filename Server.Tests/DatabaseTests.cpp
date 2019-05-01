#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Server/DataBase/PostgreSQL/DbCommandHandler.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

constexpr auto DbName = "CardGame-Test";
constexpr auto UserName = "test_user";
constexpr auto Password = "1234";

namespace ServerTests
{
    TEST_CLASS(DatabaseTests)
    {
    public:

        TEST_CLASS_INITIALIZE(Setup)
        {
            // In case some test left in DB something due to failure
            DatabaseHandler dbHandler(DbName, UserName, Password);
            PreparedStatement stmt("TRUNCATE users CASCADE;");

            dbHandler.ExecuteCommand(stmt, nullptr);
        }

        TEST_METHOD(UserCrudTests)
        {
            DbCommandHandler commandHandler(DbName, UserName, Password);

            User user;
            user.Email = "test@mail.sk";
            user.UserName = "test_user";
            user.PasswordHash = "1234";
            user.PasswordSalt = "1234";

            commandHandler.CreateUser(user);
            Assert::AreNotEqual(0, user.Id);
            User dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(user.Id, dbUser.Id);
            Assert::AreEqual(user.Email, dbUser.Email);
            Assert::AreEqual(user.UserName, dbUser.UserName);

            commandHandler.SetUserActive(user.Id, false);
            dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(0, dbUser.Id);
            commandHandler.SetUserActive(user.Id, true);
            dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(user.Id, dbUser.Id);

            commandHandler.DeleteUser(user.Id);
            dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(0, dbUser.Id);
        }
    };
}
