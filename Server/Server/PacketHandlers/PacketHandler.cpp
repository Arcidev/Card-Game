#include "PacketHandler.h"
#include "Packet.h"

PacketHandlerFunc const PacketHandler::packetHandlers[] =
{
    handleInitPacket,           // CMSG_INIT_PACKET
    handleChatPacket,           // CMSG_CHAT_MESSAGE
    handleSelectedCardsPacket,  // CMSG_SELECTED_CARDS
    handleCardActionPacket,     // CMSG_CARD_ACTION
    handleDefendSelfPacket,     // CMSG_DEFEND_SELF
    handleUserCreatePacket,     // CMSG_USER_CREATE
    handleUserLogInPacket       // CMSG_USER_LOGIN
};

// Returns function that handle sended packet
PacketHandlerFunc PacketHandler::GetPacketHandler(uint16_t packetId)
{
    return packetId < CMSG_MAX_PACKET_VALUE ? packetHandlers[packetId] : nullptr;
}
