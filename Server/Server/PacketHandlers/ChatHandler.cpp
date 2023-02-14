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
            user->GetNetwork()->BroadcastPacket(pck, user);
            break;
        case CHAT_LOCAL:
            if (Player const* player = user->GetPlayer()) // user does not have to be playing to use chat, but for game chat he should
            {
                Game const* game = player->GetGame();
                Player const* opponent = game->GetOpponent(player);
                if (opponent && !opponent->GetUser()->IsUserBlocked(user->GetDatabaseId()))
                    game->BroadcastPacket(pck);
                else
                    user->SendPacket(pck);
            }
            break;
        case CHAT_WHISPER:
        {
            std::string receiverName;
            packet >> receiverName;
            ConnectedUser* receiver = user->GetNetwork()->GetUser(receiverName);
            bool canSendMessage = receiver && !receiver->IsUserBlocked(user->GetDatabaseId());
            if (canSendMessage)
                receiver->SendPacket(pck);
            user->SendChatWhisperResponse(message, receiverName, canSendMessage);
            break;
        }
        default:
            break;
    }
}
