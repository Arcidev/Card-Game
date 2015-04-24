#pragma once
#include <map>
#include <string>
#include <vector>
#include "Card/PlayableCard.h"
#include "Game.h"
#include "../Multiplatform/NetworkCommunication.h"

typedef std::map<uint64_t, PlayableCard*> CardsMap;

class Packet;
class ServerNetwork;

class Player
{
    private:
        std::vector<PlayableCard*> m_cardOrder;
        std::vector<PlayableCard*> m_currentCards;
        bool m_isPrepared;
        bool m_isDisconnected;
        uint32_t m_id;
        CardsMap m_cards;
        uint8_t m_currentCardIndex;
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
        void IncreaseCurrentCardIndex() { ++m_currentCardIndex; };
        void SendInitResponse() const;
        void SendAvailableCards() const;
        void SendChatWhisperResponse(std::string const& message, std::string const& receiver, bool success) const;
        void SendSelectCardsFailed(uint8_t failReason) const;
        void SendPlayerDisconnected() const;
        void SendInvalidTarget() const;
        void Attack(uint64_t victimCardGuid, uint8_t attackType);
        void Defense() { GetCurrentCard()->Defend(); }
        void Prepare();
        void DestroyCard(uint64_t cardGuid);
        void ClearCards() { m_cards.clear(); }
        void CreateCard(Card const& cardTemplate);
        void ReceivePacket(uint32_t length, char const* packetData);
        void SendPacket(Packet const* packet) const;
        void Disconnect();
        void HandleDeckCards(bool addCard);

        Player* GetOpponent() const { return m_game->GetOpponent(this); }
        PlayableCard* GetCurrentCard();
        CardsMap const& GetCards() { return m_cards; }
        std::vector<PlayableCard*> const& GetCurrentCards() { return m_currentCards; }
        PlayableCard* GetCard(uint64_t cardGuid);
        SOCKET GetSocket() const { return m_socket; }
        Game* GetGame() const { return m_game; }
        uint32_t GetId() const { return m_id; }
        std::string const& GetName() const { return m_name; }
        std::string const& GetEncryptionKey() const { return m_AesKey; }
        ServerNetwork const* GetNetwork() const { return m_network; }
        bool IsPrepared() const { return m_isPrepared; }
        bool IsDisconnected() const { return m_isDisconnected; }
};
