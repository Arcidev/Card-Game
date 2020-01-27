#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../../Shared/SharedDefines.h"
#include "../../Crypto/Aes.h"
#include "../../Crypto/Rsa.h"
#include "../../Crypto/Keys/RSAPrivateKey.h"
#include "../../Crypto/OpenSSL/aes.h"

// Handle CMSG_INIT_PACKET packet
void PacketHandler::handleInitPacket(ConnectedUser* user, Packet& packet)
{
    std::vector<uint8_t> keys;
    packet >> keys;
    keys = Rsa::Decrypt(keys, privateKey, false);
    user->SetAesEncryptor(std::vector<uint8_t>(keys.begin(), keys.begin() + AES_BLOCK_SIZE * 2), std::vector<uint8_t>(keys.begin() + AES_BLOCK_SIZE * 2, keys.begin() + 3 * AES_BLOCK_SIZE ));
    user->SendInitResponse();

    /*std::vector<uint8_t> name;
    packet >> name;
    name = Aes::Decrypt(std::vector<uint8_t>(name.begin(), name.end()), player->GetAesEncryptor().Key, player->GetAesEncryptor().IVec);

    std::string nameStr = std::string((char*)name.data(), name.size());
    player->SetName(nameStr);
    
    player->SendAvailableCards();

    DEBUG_LOG("CMSG_INIT_PACKET:\r\n\tName: %s\r\n", nameStr.c_str());
    if (player->GetGame()->IsFull())
        player->GetOpponent()->SendInitResponse();*/
}
