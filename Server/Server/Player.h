#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>
#include "PlayerDefines.h"
#include "Cards/CardDefines.h"
#include "Spells/SpellDefines.h"

class Card;
class ConnectedUser;
class Game;
class Packet;
class PlayableCard;
class Spell;
class SpellAuraEffect;

typedef std::map<uint64_t, PlayableCard*> CardsMap;

class Player
{
    private:
        std::vector<PlayableCard*> m_cardOrder;
        std::vector<PlayableCard*> m_currentCards;
        bool m_isPrepared;
        uint8_t m_replenishmentMoveCount;
        uint32_t m_id;
        CardsMap m_cards;
        uint8_t m_currentCardIndex;
        Game* m_game;
        ConnectedUser* m_user;

        void destroyCard(uint64_t cardGuid);
        void replenishMana();
        void endTurn();

    public:
        Player(Game* game, ConnectedUser* user);
        ~Player();

        void SendGameInfo() const;
        void SendAvailableCards() const;
        void SendSelectCardsFailed(FailReson failReason) const;
        void SendPlayerDisconnected() const;
        void SendAttackResult(AttackResult result, uint64_t cardGuid, uint8_t damage) const;
        void EndGame(uint32_t winnerId) const;
        void SendCardStatChanged(PlayableCard const* card, CardStats cardStat) const;
        void SendMorphInfo(PlayableCard const* card) const;
        void SendApplyAura(uint64_t targetGuid, SpellAuraEffect const& aura) const;
        void SendAuraExpired(uint64_t targetGuid, SpellAuraEffect const& aura) const;
        void SendAurasRemoved(uint64_t targetGuid, std::list<uint32_t> const& spellIds) const;
        void SendSpellCastResult(SpellCastResult reason, PlayableCard const* card, Spell const* amount) const;
        void SendCardHealed(PlayableCard const* card, uint8_t amount) const;
        void Attack(uint64_t victimCardGuid);
        void SpellAttack(std::list<PlayableCard*> const& targets, uint8_t damage, bool applyDefense);
        void UseSpell(uint64_t selectedCardGuid);
        void DefendSelf();
        void Prepare();
        void DealPeriodicDamage(PlayableCard* card, uint32_t damage, bool applyDefense);
        void Drain(PlayableCard* card, uint8_t drainedHealth, uint8_t restoredHealth, uint8_t drainedMana, uint8_t restoredMana, bool applyDefense);
        bool SwapCards(PlayableCard* card, PlayableCard* other) const;

        void ClearCards();
        void CreateCard(Card const* cardTemplate);
        void SendPacket(Packet const& packet) const;
        void Disconnect() const;
        void HandleDeckCards(bool addCard);

        Player* GetOpponent() const;
        PlayableCard* GetCurrentCard();
        CardsMap const& GetCards() const { return m_cards; }
        std::vector<PlayableCard*> const& GetCurrentCards() const { return m_currentCards; }
        PlayableCard* GetCard(uint64_t cardGuid);
        Game* GetGame() const { return m_game; }
        uint32_t GetId() const { return m_id; }
        uint8_t GetCurrentCardIndex() const { return m_currentCardIndex; }
        std::string_view GetName() const;
        ConnectedUser* GetUser() const { return m_user; }
        bool IsPrepared() const { return m_isPrepared; }
        bool IsActive() const;
};
