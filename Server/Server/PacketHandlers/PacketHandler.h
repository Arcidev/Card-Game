#pragma once
#include <cstdint>
#include "Packet.h"

class ConnectedUser;

typedef void(*PacketHandlerFunc)(ConnectedUser* user, Packet& packet);
typedef bool(*HandlerValidatorFunc)(ConnectedUser const* user);

struct PacketHandlerFuncWrapper
{
    PacketHandlerFunc Handler;
    HandlerValidatorFunc HandlerValidator;

    void Invoke(ConnectedUser* user, Packet& packet)
    {
        if (Handler != nullptr && HandlerValidator != nullptr && HandlerValidator(user))
            Handler(user, packet);
    }
};

class PacketHandler
{
    private:
        static PacketHandlerFuncWrapper const packetHandlers[];

        static void handleInitPacket(ConnectedUser* user, Packet& packet);
        static void handleChatPacket(ConnectedUser* user, Packet& packet);
        static void handleStartGamePacket(ConnectedUser* user, Packet& /*packet*/);
        static void handleSelectedCardsPacket(ConnectedUser* user, Packet& packet);
        static void handleCardActionPacket(ConnectedUser* user, Packet& packet);
        static void handleDefendSelfPacket(ConnectedUser* user, Packet& /*packet*/);

        // User packets
        static void handleUserCreatePacket(ConnectedUser* user, Packet& packet);
        static void handleUserLogInPacket(ConnectedUser* user, Packet& packet);
        static void handleUserChangePassword(ConnectedUser* user, Packet& packet);

        static void handleGetAchievementsPacket(ConnectedUser* cUser, Packet& /*packet*/);
        static void handleGetUserListPacket(ConnectedUser* cUser, Packet& packet);
        static void handleUserRelationPacket(ConnectedUser* cUser, Packet& packet);

        // Validators
        static bool isLoggedIn(ConnectedUser const* user);
        static bool isPlaying(ConnectedUser const* user);
        static bool isPrepared(ConnectedUser const* user);
        static bool isInGame(ConnectedUser const* user);
        static bool notPrepared(ConnectedUser const* user) { return isPlaying(user) && !isPrepared(user); }
        static bool notLoggedIn(ConnectedUser const* user) { return !isLoggedIn(user); }
        static bool notPlaying(ConnectedUser const* user) { return isLoggedIn(user) && !isPlaying(user); }

    public:
        static PacketHandlerFuncWrapper GetPacketHandler(uint16_t packetId);
};

