#include "PacketHandler.h"
#include "Packet.h"

PacketHandlerFunc const PacketHandler::packetHandlers[] =
{
    handleInitPacket,           // CMSG_INIT_PACKET
    handleChatPacket,           // CMSG_CHAT_MESSAGE
    handleSelectedCardsPacket,  // CMSG_SELECTED_CARDS
    handleCardActionPacket,     // CMSG_CARD_ACTION
    handleDefendSelfPacket      // CMSG_DEFEND_SELF
};

// Returns function that handle sended packet
PacketHandlerFunc PacketHandler::GetPacketHandler(uint16_t const& packetId)
{
    return packetId < CMSG_MAX_PACKET_VALUE ? packetHandlers[packetId] : nullptr;
}
