#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../Game.h"
#include "../Player.h"
#include "../../Shared/SharedDefines.h"
#include "../../Crypto/Aes.h"
#include "../../Crypto/Rsa.h"
#include "../../Crypto/Keys/RSAPrivateKey.h"
#include "../../Crypto/OpenSSL/aes.h"

// Handle CMSG_INIT_PACKET packet
void PacketHandler::handleInitPacket(ConnectedUser* user, Packet& packet)
{
    DEBUG_LOG("CMSG_INIT_PACKET:\r\n");

    std::vector<uint8_t> keys;
    packet >> keys;
    keys = Rsa::Decrypt(keys, privateKey, false);
    user->SetAesEncryptor(std::vector<uint8_t>(keys.begin(), keys.begin() + AES_BLOCK_SIZE * 2), std::vector<uint8_t>(keys.begin() + AES_BLOCK_SIZE * 2, keys.begin() + 3 * AES_BLOCK_SIZE ));
    user->SendInitResponse();
}

// Handle CMSG_START_GAME packet
void PacketHandler::handleStartGamePacket(ConnectedUser* user, Packet& /*packet*/)
{
    DEBUG_LOG("CMSG_START_GAME:\r\n");

    user->CreatePlayer();
    Player* player = user->GetPlayer();

    player->SendGameInfo();
    player->SendAvailableCards();
    if (player->GetGame()->IsFull())
        player->GetOpponent()->SendGameInfo();
}
