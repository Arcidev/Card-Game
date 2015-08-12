#include "PacketHandler.h"
#include "Packet.h"
#include "../Player.h"
#include "../../Shared/SharedDefines.h"
#include "../../Crypto/Aes.h"
#include "../../Crypto/Rsa.h"
#include "../../Crypto/Keys/RSAPrivateKey.h"
#include "../../Crypto/OpenSSL/aes.h"

// Handle CMSG_INIT_PACKET packet
void PacketHandler::handleInitPacket(Player* player, Packet* packet)
{
    std::string keys;
    *packet >> keys;
    keys = Rsa::Decrypt(keys, privateKey, false);
    player->SetAesEncryptor(keys.substr(0, AES_BLOCK_SIZE), std::vector<uint8_t>(keys.begin() + AES_BLOCK_SIZE, keys.begin() + 2 * AES_BLOCK_SIZE ));

    std::string name;
    *packet >> name;
    player->SetName(Aes::Decrypt(name, player->GetAesEncryptor().Key, player->GetAesEncryptor().IVec).c_str() /*to trim \0 at the end*/);
    player->SendInitResponse();
    player->SendAvailableCards();

    DEBUG_LOG("CMSG_INIT_PACKET:\n\tName: %s\n", player->GetName().c_str());
    if (player->GetGame()->IsFull())
        player->GetOpponent()->SendInitResponse();
}