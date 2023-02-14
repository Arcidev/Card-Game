#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Server/Crypto/Sha.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

constexpr auto Password = "123456";

namespace ServerTests
{
    TEST_CLASS(SecurityTests)
    {
    public:

        TEST_METHOD(HashingTest)
        {
            auto[salt, hash] = Sha::CreateHash(Password);
            Assert::IsFalse(salt.empty());
            Assert::IsFalse(hash.empty());
            Assert::AreEqual(64, (int)salt.length());
            Assert::AreEqual(128, (int)hash.length());

            auto saltedHash = Sha::CreateHash(Password, salt);
            Assert::AreEqual(hash, saltedHash);
        }
    };
}
