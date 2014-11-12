#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <qstring.h>
#include "ClientNetwork.h"

class CardGame;

class ClientGame
{
    private:
        QString m_name;
        ClientNetwork m_network;
        CardGame* m_cardGame;
        std::string m_AesKey;
        char network_data[MAX_PACKET_SIZE];
        
        void handleInitPacket(Packet& packet);

    public:
        ClientGame(CardGame* cardGame, QString const& playerName);

        void Update();
        void SendPacket(Packet const& packet) const;
};

