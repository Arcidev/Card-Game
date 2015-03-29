#pragma once
#include <map>
#include <string>
#include "Card.h"
#include "../Multiplatform/NetworkCommunication.h"

typedef std::map<uint64_t, Card> CardsMap;

class Game;
class Packet;
class ServerNetwork;

class Player
{
    private:
        uint32_t m_id;
        CardsMap m_cards;
        Card* m_currentCard;
        Game* m_game;
        ServerNetwork* m_network;
        SOCKET m_socket;
        std::string m_name;
        std::string m_AesKey;

    public:
        Player(uint32_t id, SOCKET socket, Game* game, ServerNetwork* network);
        ~Player();

        void SetEncryptionKey(std::string const& key) { m_AesKey = key; }
        void SetName(std::string const& name) { m_name = name; }

        void SendInitResponse() const;
        void SendAvailableCards() const;
        void SendChatWhisperResponse(std::string const& message, std::string const& receiver, bool success) const;
        void SendSelectCardsFailed(uint8_t failReason) const;

        void Attack(Player* victim, uint64_t victimCardGuid);
        void Defense() { m_currentCard->Defend(); }
        void DestroyCard(uint64_t cardGuid);
        void ReceivePacket(uint32_t length, char const* packetData);
        void SendPacket(Packet const* packet) const;
        
        Card* GetCard(uint64_t cardGuid);
        SOCKET GetSocket() const { return m_socket; }
        Game* GetGame() const { return m_game; }
        uint32_t GetID() const { return m_id; }
        std::string const& GetName() const { return m_name; }
        std::string const& GetEncryptionKey() const { return m_AesKey; }
        ServerNetwork const* GetNetwork() const { return m_network; }
};
