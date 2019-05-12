#pragma once
#include <cstdint>
#include "Packet.h"

class Player;

typedef void(*PacketHandlerFunc)(Player* player, Packet& packet);

class PacketHandler
{
    private:
        static PacketHandlerFunc const packetHandlers[];

        static void handleInitPacket(Player* player, Packet& packet);
        static void handleChatPacket(Player* player, Packet& packet);
        static void handleSelectedCardsPacket(Player* player, Packet& packet);
        static void handleCardActionPacket(Player* player, Packet& packet);
        static void handleDefendSelfPacket(Player* player, Packet& packet);

        // User packets
        static void handleUserCreatePacket(Player* player, Packet& packet);
        static void handleUserLogInPacket(Player* player, Packet& packet);
        static void handleUserChangePassword(Player* player, Packet& packet);

    public:
        static PacketHandlerFunc GetPacketHandler(uint16_t packetId);
};

