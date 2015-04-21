#pragma once
#include <cstdint>
#include <map>

class Player;
class Packet;

typedef void(*PacketHandlerFunc)(Player* player, Packet* packet);
typedef std::map<uint16_t, PacketHandlerFunc> PacketHandlerMap;

class PacketHandler
{
    private:
        static PacketHandlerMap const packetHandlers;

        static void handleInitPacket(Player* player, Packet* packet);
        static void handleChatPacket(Player* player, Packet* packet);
        static void handleSelectedCardsPacket(Player* player, Packet* packet);
        static void handleAttackCardPacket(Player* player, Packet* packet);

    public:
        static PacketHandlerFunc GetPacketHandler(uint16_t packetId);
};

