#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>
#include "Game.h"
#include "../Multiplatform/NetworkCommunication.h"

class Card;
class Packet;
class Spell;
class ServerNetwork;
class PlayableCard;
class SpellAuraEffect;

typedef std::map<uint64_t, PlayableCard*> CardsMap;

struct AesEncryptor
{
    std::string Key;
    std::vector<uint8_t> IVec;
};
class Player
{
    private:
        std::vector<PlayableCard*> m_cardOrder;
        std::vector<PlayableCard*> m_currentCards;
        bool m_isPrepared;
        bool m_isDisconnected;
        uint8_t m_replenishmentMoveCount;
        uint32_t m_id;
        CardsMap m_cards;
        uint8_t m_currentCardIndex;
        Game* m_game;
        ServerNetwork* m_network;
        SOCKET m_socket;
        std::string m_name;
        AesEncryptor m_AesEncryptor;

        void destroyCard(uint64_t const& cardGuid);
        void replenishMana();
        void endTurn();

    public:
        Player(uint32_t id, SOCKET socket, Game* game, ServerNetwork* network);
        ~Player();

        void SetAesEncryptor(std::string const& key, std::vector<uint8_t> const& iVec) { m_AesEncryptor.Key = key; m_AesEncryptor.IVec = iVec; }
        void SetName(std::string const& name) { m_name = name; }
        void SendInitResponse() const;
        void SendAvailableCards() const;
        void SendChatWhisperResponse(std::string const& message, std::string const& receiver, bool success) const;
        void SendSelectCardsFailed(uint8_t const& failReason) const;
        void SendPlayerDisconnected() const;
        void SendAttackResult(uint8_t const& result, uint64_t const& cardGuid, uint8_t const& damage) const;
        void SendEndGame(uint32_t const& winnerId) const;
        void SendCardStatChanged(PlayableCard const* card, uint8_t const& cardStat) const;
        void SendApplyAura(uint64_t const& targetGuid, SpellAuraEffect const* aura) const;
        void SendSpellCastResult(uint8_t const& reason, PlayableCard const* card, Spell const* amount) const;
        void SendCardHealed(PlayableCard const* card, uint8_t const& amount) const;
        void Attack(uint64_t const& victimCardGuid);
        void SpellAttack(std::list<PlayableCard*> const& targets, uint8_t const& damage);
        void UseSpell(uint64_t const& selectedCardGuid);
        void DefendSelf();
        void Prepare();
        void DealPeriodicDamage(PlayableCard* card, uint32_t const& damage);
        
        void ClearCards() { m_cards.clear(); }
        void CreateCard(Card const* cardTemplate);
        void ReceivePacket(uint32_t const& length, char const* packetData);
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
        AesEncryptor const& GetAesEncryptor() const { return m_AesEncryptor; }
        ServerNetwork const* GetNetwork() const { return m_network; }
        ServerNetwork* GetNetwork() { return m_network; }
        bool IsPrepared() const { return m_isPrepared; }
        bool IsDisconnected() const { return m_isDisconnected; }
        bool IsActive() const { return m_game->GetActivePlayerId() == m_id; }
};
