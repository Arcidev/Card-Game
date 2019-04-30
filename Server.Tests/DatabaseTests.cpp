#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Server/DataBase/PostgreSQL/DbCommandHandler.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTests
{		
	TEST_CLASS(DatabaseTests)
	{
	public:
		
		TEST_METHOD(UserCrudTests)
		{
            DbCommandHandler commandHandler("CardGame-Test", "test_user", "1234");

            User user;
            user.Email = "test@mail.sk";
            user.UserName = "test_user";
            user.PasswordHash = "1234";
            user.PasswordSalt = "1234";

            commandHandler.CreateUser(user);
            Assert::AreNotEqual(0, user.Id);
            User dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(user.Id, dbUser.Id);

            commandHandler.DeleteUser(user.Id);
            dbUser = commandHandler.GetUser(user.Email);
            Assert::AreEqual(0, dbUser.Id);
		}
	};
}
