#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../Player.h"

PacketHandlerFuncWrapper const PacketHandler::packetHandlers[] =
{
    { handleInitPacket, notLoggedIn },          // CMSG_INIT_PACKET
    { handleChatPacket, isLoggedIn },           // CMSG_CHAT_MESSAGE
    { handleStartGamePacket, notPlaying },      // CMSG_START_GAME
    { handleSelectedCardsPacket, notPrepared }, // CMSG_SELECTED_CARDS
    { handleCardActionPacket, isInGame },       // CMSG_CARD_ACTION
    { handleDefendSelfPacket, isInGame },       // CMSG_DEFEND_SELF
    { handleUserCreatePacket, notLoggedIn },    // CMSG_USER_CREATE
    { handleUserLogInPacket, notLoggedIn },     // CMSG_USER_LOGIN
    { handleUserChangePassword, isLoggedIn }    // CMSG_USER_CHANGE_PASSWORD
};

inline bool PacketHandler::isLoggedIn(ConnectedUser const* user)
{
    return user->GetDatabaseId();
}

inline bool PacketHandler::isPlaying(ConnectedUser const* user)
{
    return user->GetPlayer();
}

inline bool PacketHandler::isPrepared(ConnectedUser const* user)
{
    return user->GetPlayer() && user->GetPlayer()->IsPrepared();
}

inline bool PacketHandler::isInGame(ConnectedUser const* user)
{
    // TODO
    return isPrepared(user);
}

// Returns function that handle sended packet
PacketHandlerFuncWrapper PacketHandler::GetPacketHandler(uint16_t packetId)
{
    if (packetId < (uint16_t)CMSGPackets::CMSG_MAX_PACKET_VALUE)
        return packetHandlers[packetId];
    
    return PacketHandlerFuncWrapper { nullptr, nullptr };
}
