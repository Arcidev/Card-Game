#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../Player.h"

PacketHandlerFuncWrapper const PacketHandler::packetHandlers[] =
{
    { handleInitPacket, notLoggedIn },          // CMSG_INIT_PACKET
    { handleChatPacket, isLoggedIn },           // CMSG_CHAT_MESSAGE
    { handleSelectedCardsPacket, notInGame },   // CMSG_SELECTED_CARDS
    { handleCardActionPacket, isInGame },       // CMSG_CARD_ACTION
    { handleDefendSelfPacket, isInGame },       // CMSG_DEFEND_SELF
    { handleUserCreatePacket, notLoggedIn },    // CMSG_USER_CREATE
    { handleUserLogInPacket, notLoggedIn },     // CMSG_USER_LOGIN
    { handleUserChangePassword, isLoggedIn }    // CMSG_USER_CHANGE_PASSWORD
};

inline bool PacketHandler::isLoggedIn(ConnectedUser const* user)
{
    return user->IsLoggedIn();
}

inline bool PacketHandler::isInGame(ConnectedUser const* user)
{
    return user->GetPlayer()->GetGame() != nullptr;
}

inline bool PacketHandler::notInGame(ConnectedUser const* user)
{
    return isLoggedIn(user) && !isInGame(user);
}

// Returns function that handle sended packet
PacketHandlerFuncWrapper PacketHandler::GetPacketHandler(uint16_t packetId)
{
    if (packetId < (uint16_t)CMSGPackets::CMSG_MAX_PACKET_VALUE)
        return packetHandlers[packetId];
    
    return PacketHandlerFuncWrapper { nullptr, nullptr };
}
