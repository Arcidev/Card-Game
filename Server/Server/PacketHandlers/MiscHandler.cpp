#include "PacketHandler.h"
#include "Packet.h"
#include "../Player.h"
#include "../../Shared/SharedDefines.h"
#include "../../Crypto/Aes.h"
#include "../../Crypto/Rsa.h"
#include "../../Crypto/Keys/RSAPrivateKey.h"

// Handle CMSG_INIT_PACKET packet
void PacketHandler::handleInitPacket(Player* player, Packet* packet)
{
    std::string aesKey;
    *packet >> aesKey;
    aesKey = Rsa::Decrypt(aesKey, privateKey, false);
    player->SetEncryptionKey(aesKey);

    std::string name;
    *packet >> name;
    player->SetName(Aes::Decrypt(name, player->GetEncryptionKey()).c_str() /*to trim \0 at the end*/);
    player->SendInitResponse();
    player->SendAvailableCards();

    DEBUG_LOG("CMSG_INIT_PACKET:\n\tName: %s\n", player->GetName().c_str());
    if (player->GetGame()->IsFull())
        player->GetOpponent()->SendInitResponse();
}