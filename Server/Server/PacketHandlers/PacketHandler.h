#pragma once
#include <cstdint>

class Player;
class Packet;

typedef void(*PacketHandlerFunc)(Player* player, Packet* packet);

class PacketHandler
{
    private:
        static PacketHandlerFunc const packetHandlers[];

        static void handleInitPacket(Player* player, Packet* packet);
        static void handleChatPacket(Player* player, Packet* packet);
        static void handleSelectedCardsPacket(Player* player, Packet* packet);
        static void handleCardActionPacket(Player* player, Packet* packet);
        static void handleDefendSelfPacket(Player* player, Packet* packet);

    public:
        static PacketHandlerFunc GetPacketHandler(uint16_t const& packetId);
};

