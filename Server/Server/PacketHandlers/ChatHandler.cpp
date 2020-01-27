#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../Player.h"
#include "../ServerNetwork.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_CHAT_MESSAGE packet
void PacketHandler::handleChatPacket(ConnectedUser* user, Packet& packet)
{
    uint8_t chatId;
    std::string message;
    packet >> chatId >> message;

    DEBUG_LOG("CMSG_CHAT_MESSAGE:\r\n\tChatId: %d\r\n\tMessage: %s\r\n", chatId, message.c_str());
    Packet pck(SMSGPackets::SMSG_CHAT_MESSAGE);
    pck << chatId;
    pck << user->GetName();
    pck << message;

    switch (chatId)
    {
        case CHAT_GLOBAL:
            user->GetNetwork()->BroadcastPacket(pck);
            break;
        case CHAT_LOCAL:
            if (user->GetPlayer())
                user->GetPlayer()->GetGame()->BroadcastPacket(pck);
            break;
        case CHAT_WHISPER:
        {
            std::string receiverName;
            packet >> receiverName;
            user->SendChatWhisperResponse(message, receiverName, user->GetNetwork()->SendPacketToPlayer(receiverName, pck));
            break;
        }
        default:
            break;
    }
}
