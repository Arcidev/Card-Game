#include "PacketHandler.h"
#include "Packet.h"
#include "../Player.h"
#include "../ServerNetwork.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_CHAT_MESSAGE packet
void PacketHandler::handleChatPacket(Player* player, Packet* packet)
{
    uint8_t chatId;
    std::string message;
    *packet >> chatId;
    *packet >> message;

    DEBUG_LOG("CMSG_CHAT_MESSAGE:\r\n\tChatId: %d\r\n\tMessage: %s\r\n", chatId, message.c_str());
    Packet pck(SMSG_CHAT_MESSAGE);
    pck << chatId;
    pck << player->GetName();
    pck << message;

    switch (chatId)
    {
        case CHAT_GLOBAL:
            player->GetNetwork()->BroadcastPacket(pck);
            break;
        case CHAT_LOCAL:
            player->GetGame()->BroadcastPacket(pck);
            break;
        case CHAT_WHISPER:
        {
            std::string receiverName;
            *packet >> receiverName;
            player->SendChatWhisperResponse(message, receiverName, player->GetNetwork()->SendPacketToPlayer(receiverName, pck));
            break;
        }
        default:
            break;
    }
}
