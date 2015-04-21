#include "PacketHandler.h"
#include "Packet.h"

PacketHandlerMap const PacketHandler::packetHandlers =
{
    { CMSG_INIT_PACKET,     handleInitPacket            },
    { CMSG_CHAT_MESSAGE,    handleChatPacket            },
    { CMSG_SELECTED_CARDS,  handleSelectedCardsPacket   },
    { CMSG_ATTACK_CARD,     handleAttackCardPacket      }
};

// Returns function that handle sended packet
PacketHandlerFunc PacketHandler::GetPacketHandler(uint16_t packetId)
{
    PacketHandlerMap::const_iterator iter = packetHandlers.find(packetId);
    return (iter != packetHandlers.end()) ? iter->second : nullptr;
}
