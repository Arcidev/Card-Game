#include <algorithm>
#include <random>
#include "ConnectedUser.h"
#include "DataHolder.h"
#include "Game.h"
#include "Player.h"
#include "ServerNetwork.h"
#include "Cards/PlayableCard.h"
#include "PacketHandlers/PacketHandler.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuraEffect.h"

static uint8_t calculateReducedDamage(uint8_t damage, uint8_t defense)
{
    float reduction = (defense * (float)SystemStats::DEFENSE_PERCENT_PER_POINT);
    if (!reduction)
        return damage;

    reduction /= 100.f;
    reduction += 1.f;
    return (uint8_t)(damage / reduction);
}

Player::Player(Game* game, ConnectedUser* user)
    : m_isPrepared(false), m_replenishmentMoveCount(0), m_currentCardIndex(0), m_game(game), m_user(user) { }

Player::~Player()
{
    for (auto& card : m_cards)
        delete card.second;

    m_user->UnsetPlayer();
    m_user->GetNetwork()->OnPlayerDisconnected(this);
}

// Set player state to disconnected
void Player::Disconnect() const
{
    m_game->DisconnectPlayer(GetId());
    m_user->GetNetwork()->OnPlayerDisconnected(this);
}

// Sends attack result
void Player::SendAttackResult(AttackResult result, uint64_t cardGuid, uint8_t damage) const
{
    Packet packet(SMSGPackets::SMSG_ATTACK_RESULT);
    packet << (uint8_t)result;
    if (result != AttackResult::ATTACK_RESULT_INVALID_TARGET)
    {
        packet.WriteBitStreamInOrder(cardGuid, { 6, 2, 1, 7, 3, 0, 4, 5 });
        packet.FlushBits();
        packet.WriteByteStreamInOrder(cardGuid, { 2, 6, 7 });
        packet << GetId();
        packet.WriteByteStreamInOrder(cardGuid, { 1, 3, 0 });
        packet << damage;
        packet.WriteByteStreamInOrder(cardGuid, { 5, 4 });
        GetGame()->BroadcastPacket(packet);
    }
    else
        SendPacket(packet);
}

// Attacks enemy player
void Player::Attack(uint64_t victimCardGuid)
{
    if (!IsActive())
        return;

    Player* victim = GetGame()->GetOpponent(this);
    if (!victim)
        return;

    PlayableCard* victimCard = victim->GetCard(victimCardGuid);
    PlayableCard* currentCard = GetCurrentCard();
    if (!victimCard || !currentCard)
        return;

    if (!currentCard->CanAttackCard(victimCardGuid, victim->GetCurrentCards(), m_currentCardIndex))
    {
        SendAttackResult(AttackResult::ATTACK_RESULT_INVALID_TARGET, 0, 0);
        return;
    }

    uint8_t damage = calculateReducedDamage(currentCard->GetModifiedDamage(victimCard->GetStatModifierValue(CardStats::CARD_STAT_DAMAGE_TAKEN)), victimCard->GetModifiedDefense());
    victimCard->DealDamage(damage);

    if (!victimCard->IsAlive())
    {
        victim->destroyCard(victimCardGuid);
        SendAttackResult(AttackResult::ATTACK_RESULT_CARD_DESTROYED, victimCardGuid, damage);
        if (victim->m_cardOrder.empty() && victim->m_currentCards.empty())
        {
            EndGame(GetId());
            return;
        }
        else
            victim->HandleDeckCards(victim->m_currentCards.empty() ? true : false);
    }
    else
        SendAttackResult(AttackResult::ATTACK_RESULT_CARD_ATTACKED, victimCardGuid, damage);

    endTurn();
}

// Attack enemy with spell
void Player::SpellAttack(std::list<PlayableCard*> const& targets, uint8_t damage, bool applyDefense)
{
    Player* victim = GetGame()->GetOpponent(this);
    if (!victim)
        return;

    bool sendOpponentCardDeck = false;
    Packet packet(SMSGPackets::SMSG_SPELL_DAMAGE);
    ByteBuffer buffer;
    packet << (uint8_t)targets.size();

    uint8_t attackerModifier = GetCurrentCard()->GetStatModifierValue(CardStats::CARD_STAT_SPELL_DAMAGE);
    for (PlayableCard* target : targets)
    {
        damage = (std::max)(damage + attackerModifier + target->GetStatModifierValue(CardStats::CARD_STAT_SPELL_DAMAGE_TAKEN), 0);
        if (applyDefense)
            damage = calculateReducedDamage(damage, target->GetModifiedDefense());

        target->DealDamage(damage);
        if (target->IsAlive())
            packet.WriteBit(true);
        else
        {
            if (!sendOpponentCardDeck)
                sendOpponentCardDeck = true;
            victim->destroyCard(target->GetGuid());

            packet.WriteBit(false);
        }
        
        packet.WriteBitStreamInOrder(target->GetGuid(), { 6, 3, 1, 7, 0, 2, 5, 4 });

        buffer.WriteByteStreamInOrder(target->GetGuid(), { 4, 3, 5 });
        buffer << damage;
        buffer.WriteByteStreamInOrder(target->GetGuid(), { 2, 0, 1, 6, 7 });
    }

    packet.FlushBits();
    packet << GetId();
    packet << buffer;

    GetGame()->BroadcastPacket(packet);

    if (sendOpponentCardDeck)
    {
        if (victim->m_cardOrder.empty() && victim->m_currentCards.empty())
            EndGame(GetId());
        else
            victim->HandleDeckCards(victim->m_currentCards.empty() ? true : false);
    }
}

// Sends information about failed spell cast to client
void Player::SendSpellCastResult(SpellCastResult reason, PlayableCard const* card, Spell const* spell) const
{
    Packet packet(SMSGPackets::SMSG_SPELL_CAST_RESULT);
    packet << (uint8_t)reason;
    if (reason != SpellCastResult::SPELL_CAST_RESULT_SUCCESS && reason != SpellCastResult::SPELL_CAST_RESULT_SUCCESS_CONTINUE_MOVE)
    {
        SendPacket(packet);
        return;
    }

    if (!card || !spell)
        return;

    packet.WriteBitStreamInOrder(card->GetGuid(), { 5, 7, 0, 1, 4, 3, 2, 6 });
    packet.FlushBits();

    packet << card->GetMana();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 7, 2 });
    packet << spell->GetManaCost();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 4, 0, 1 });
    packet << GetId();
    packet << spell->GetId();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 3, 6, 5 });
    GetGame()->BroadcastPacket(packet);
}

// Sends information that card has been healed
void Player::SendCardHealed(PlayableCard const* card, uint8_t amount) const
{
    if (!card)
        return;

    Packet packet(SMSGPackets::SMSG_CARD_HEALED);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 7, 2, 6, 1, 3, 0, 5, 4 });
    packet.FlushBits();

    packet.WriteByteStreamInOrder(card->GetGuid(), { 5, 2, 7, 1 });
    packet << GetId();
    packet << card->GetHealth();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 4, 0, 3, 6 });
    packet << amount;

    GetGame()->BroadcastPacket(packet);
}

// Uses card spell
void Player::UseSpell(uint64_t selectedCardGuid)
{
    if (!IsActive())
        return;

    PlayableCard* currentCard = GetCurrentCard();
    Player* victim = GetGame()->GetOpponent(this);
    if (!victim || !currentCard)
        return;

    if (!currentCard->GetSpell())
    {
        SendSpellCastResult(SpellCastResult::SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS, nullptr, nullptr);
        return;
    }

    Spell const* spell = currentCard->GetSpell();
    SpellCastResult result = spell->Cast(this, victim, selectedCardGuid);
    if (result != SpellCastResult::SPELL_CAST_RESULT_SUCCESS && result != SpellCastResult::SPELL_CAST_RESULT_SUCCESS_CONTINUE_MOVE)
    {
        SendSpellCastResult(result, nullptr, nullptr);
        return;
    }

    // Spell might have been changed and mana cost handled differently
    if (currentCard->GetSpell() == spell)
        currentCard->SubtractMana(spell->GetManaCost());

    SendSpellCastResult(result, currentCard, spell);

    if (result == SpellCastResult::SPELL_CAST_RESULT_SUCCESS_CONTINUE_MOVE)
    {
        GetGame()->SendActivePlayer(GetCurrentCard());
        return;
    }
        
    endTurn();
}

// Removes card from player deck
void Player::destroyCard(uint64_t cardGuid)
{
    for (uint32_t i = 0; i < m_currentCards.size(); ++i)
    {
        if (m_currentCards[i]->GetGuid() == cardGuid)
        {
            m_currentCards.erase(m_currentCards.begin() + i);

            if (m_currentCardIndex >= i)
                --m_currentCardIndex;
            break;
        }
    }
}

// Gets card from player deck
PlayableCard* Player::GetCard(uint64_t cardGuid)
{
    CardsMap::iterator iter = m_cards.find(cardGuid);
    return (iter == m_cards.end()) ? nullptr : iter->second;
}

inline uint32_t Player::GetId() const
{
    return m_user->GetId();
}

inline std::string_view Player::GetName() const
{
    return m_user->GetName();
}

inline Player* Player::GetOpponent() const
{
    return m_game->GetOpponent(this);
}

inline bool Player::IsActive() const
{
    return m_game->GetActivePlayer() == this;
}

void Player::SendGameInfo() const
{
    Player const* opponent = GetOpponent();
    Packet packet(SMSGPackets::SMSG_GAME_INFO);
    packet.WriteBit(opponent != nullptr);
    packet.FlushBits();

    packet << GetId();
    if (opponent)
    {
        packet << opponent->GetId();
        packet << opponent->GetName();
    }

    SendPacket(packet);
}

// Sends all cards that are currently available to be played with
void Player::SendAvailableCards() const
{
    // can be static, always sending the same
    CardsDataMap cards = DataHolder::GetCards();
    Packet packet(SMSGPackets::SMSG_AVAILABLE_CARDS);
    packet << (uint16_t)cards.size();

    ByteBuffer buffer;
    for (auto const& card : cards)
    {
        Spell const* spell = card.second.GetSpell();
        packet.WriteBit(spell ? true : false);

        buffer << card.second.GetId();
        buffer << (uint8_t)card.second.GetType();
        buffer << card.second.GetHealth();

        if (spell)
        {
            buffer << spell->GetManaCost();
            buffer << spell->GetId();
            buffer << (uint8_t)spell->GetSpellEffects().size();
            for (auto const& spellEffect : spell->GetSpellEffects())
            {
                buffer << spellEffect.second.Target;
                buffer << spellEffect.second.SpellAttributes;
            }
        }

        buffer << card.second.GetDamage();
        buffer << card.second.GetMana();
        buffer << card.second.GetDefense();
        buffer << card.second.GetPrice();
    }

    packet.FlushBits();
    packet << buffer;

    SendPacket(packet);
}

// Sends selection card has failed
void Player::SendSelectCardsFailed(FailReson failReason) const
{
    Packet packet(SMSGPackets::SMSG_SELECT_CARDS_FAILED);
    packet << (uint8_t)failReason;

    SendPacket(packet);
}

// Sends information about disconnected opponent
void Player::SendPlayerDisconnected() const
{
    Packet packet(SMSGPackets::SMSG_PLAYER_DISCONNECTED);
    SendPacket(packet);
}

// Sends encrypted packet to client
inline void Player::SendPacket(Packet const& packet) const
{
    m_user->SendPacket(packet);
}

// Clears player cards
void Player::ClearCards()
{
    for (const auto& cardPair : m_cards)
        delete cardPair.second;

    m_cards.clear();
}

// Adds card into deck from existing template
void Player::CreateCard(Card const* cardTemplate)
{
    if (!cardTemplate)
        return;

    PlayableCard* card = PlayableCard::Create(m_game->GetNextCardGuid(), cardTemplate, this);

    m_cards.insert(std::make_pair(card->GetGuid(), card));
    m_cardOrder.push_back(card);
}

// Set player state to prepared to play
void Player::Prepare()
{
    m_isPrepared = true;
    std::shuffle(m_cardOrder.begin(), m_cardOrder.end(), std::default_random_engine(rand()));
}

// Sends players card deck
void Player::HandleDeckCards(bool addCard)
{
    if (addCard && !m_cardOrder.empty() && (m_currentCards.size() < MAX_CARDS_ON_DECK))
    {
        m_currentCards.push_back(m_cardOrder.front());
        m_cardOrder.erase(m_cardOrder.begin());
    }
    
    uint8_t cardsCount = (uint8_t)m_currentCards.size();
    Packet packet(SMSGPackets::SMSG_DECK_CARDS);
    packet << cardsCount;
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteBitStreamInOrder(m_currentCards[i]->GetGuid(), { 7, 2, 1, 4, 5, 0, 6, 3 });
    packet.FlushBits();

    packet << GetId();
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteByteStreamInOrder(m_currentCards[i]->GetGuid(), { 2, 1, 7, 6, 0, 5, 3, 4 });
    
    m_game->BroadcastPacket(packet);
}

// Gets current card
PlayableCard* Player::GetCurrentCard()
{
    if (m_currentCards.empty())
        return nullptr;

    m_currentCardIndex = m_currentCardIndex % m_currentCards.size();
    return m_currentCards[m_currentCardIndex];
}

// Informs player that game has ended
void Player::EndGame(uint32_t winnerId) const
{
    if (GetId() == winnerId)
    {
        m_user->OnGameWon();
        GetOpponent()->GetUser()->OnGameEnded();
    }
    else
    {
        m_user->OnGameEnded();
        GetOpponent()->GetUser()->OnGameWon();
    }

    Packet packet(SMSGPackets::SMSG_END_GAME);
    packet << winnerId;

    GetGame()->BroadcastPacket(packet);
    delete GetGame();
}

// Sets active defend statte on current card
void Player::DefendSelf()
{
    if (!IsActive() || !GetCurrentCard())
        return;

    GetCurrentCard()->SetDefendState(true);
    endTurn();
}

// Sends new card stat modifiers
void Player::SendCardStatChanged(PlayableCard const* card, CardStats cardStat) const
{
    Packet packet(SMSGPackets::SMSG_CARD_STAT_CHANGED);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 2, 6, 7, 1, 0, 3, 5, 4 });
    packet.FlushBits();

    packet.WriteByteStreamInOrder(card->GetGuid(), { 5, 7 });
    packet << card->GetStatModifierValue(cardStat);
    packet.WriteByteStreamInOrder(card->GetGuid(), { 6, 3, 1 });
    packet << GetId();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 0, 4, 2 });
    packet << (uint8_t)cardStat;

    m_game->BroadcastPacket(packet);
}

// Sends info about card morph
void Player::SendMorphInfo(PlayableCard const* card) const
{
    Card const* cardInfo = card->GetMorph() ? card->GetMorph() : card;

    Packet packet(SMSGPackets::SMSG_CARD_MORPH_INFO);
    packet.WriteBit(card->GetMorph() != nullptr);
    packet.FlushBits();

    packet << GetId();
    packet.WriteBitStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteByteStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet << cardInfo->GetId();
    packet << card->GetMana();

    m_game->BroadcastPacket(packet);
}

// Sends information about aura application
void Player::SendApplyAura(uint64_t targetGuid, SpellAuraEffect const& aura) const
{
    Packet packet(SMSGPackets::SMSG_APPLY_AURA);
    packet.WriteBitStreamInOrder(targetGuid, { 7, 2, 1, 3, 5, 4, 0, 6 });
    packet.FlushBits();

    packet << GetId();
    packet.WriteByteStreamInOrder(targetGuid, { 0, 5, 2, 1, 7, 6, 4, 3 });
    packet << aura.GetSpellId();

    GetGame()->BroadcastPacket(packet);
}

// Sends information about aura expiration
void Player::SendAuraExpired(uint64_t targetGuid, SpellAuraEffect const& aura) const
{
    Packet packet(SMSGPackets::SMSG_SPELL_AURA_EXPIRED);
    packet.WriteBitStreamInOrder(targetGuid, { 0, 5, 7, 2, 1, 4, 3, 6 });
    packet.FlushBits();

    packet << aura.GetSpellId();
    packet.WriteByteStreamInOrder(targetGuid, { 7, 6, 5, 4, 3, 2, 1, 0 });
    packet << GetId();

    GetGame()->BroadcastPacket(packet);
}

// Sends information about aura expiration
void Player::SendAurasRemoved(uint64_t targetGuid, std::list<uint32_t> const& spellIds) const
{
    if (spellIds.empty())
        return;

    Packet packet(SMSGPackets::SMSG_SPELL_AURAS_REMOVED);
    packet.WriteBitStreamInOrder(targetGuid, { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteByteStreamInOrder(targetGuid, { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet << GetId();

    packet << (uint8_t)spellIds.size();
    for (uint32_t spellId : spellIds)
        packet << spellId;

    GetGame()->BroadcastPacket(packet);
}

// Replenishes mana
void Player::replenishMana()
{
    m_replenishmentMoveCount = (m_replenishmentMoveCount + 1) % (uint8_t)SystemStats::MANA_REPLENISHMENT_MOVES;
    if (!m_replenishmentMoveCount)
    {
        Packet packet(SMSGPackets::SMSG_MANA_REPLENISHMENT);
        ByteBuffer buffer;

        packet << (uint8_t)m_currentCards.size();
        buffer << GetId();
        buffer << (uint8_t)SystemStats::MANA_REPLENISHMENT_VALUE;

        for (PlayableCard* currentCard : m_currentCards)
        {
            currentCard->AddMana((uint8_t)SystemStats::MANA_REPLENISHMENT_VALUE);

            packet.WriteBitStreamInOrder(currentCard->GetGuid(), { 5, 0, 1, 2, 3, 7, 4, 6 });

            buffer.WriteByteStreamInOrder(currentCard->GetGuid(), { 2, 6, 0, 7, 1, 4, 3, 5 });
            buffer << currentCard->GetMana();
        }

        packet.FlushBits();
        packet << buffer;

        GetGame()->BroadcastPacket(packet);
    }
}

// Handles periodic damage from aura
void Player::DealPeriodicDamage(PlayableCard* card, uint32_t damage, bool applyDefense) const
{
    damage = applyDefense ? calculateReducedDamage(damage, card->GetModifiedDefense()) : damage;
    card->DealDamage(damage);

    Packet packet(SMSGPackets::SMSG_SPELL_PERIODIC_DAMAGE);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 6, 4, 1 });
    packet.WriteBit(card->IsAlive());
    packet.WriteBitStreamInOrder(card->GetGuid(), { 7, 2, 3, 5, 0 });
    packet.FlushBits();

    packet << GetId();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 7, 5, 4, 6 });
    packet << damage;
    GetGame()->BroadcastPacket(packet);
}

// Handles drain spell effect
void Player::Drain(PlayableCard* card, uint8_t drainedHealth, uint8_t restoredHealth, uint8_t drainedMana, uint8_t restoredMana, bool applyDefense)
{
    auto currentCard = GetCurrentCard();
    uint8_t damage = (std::max)(drainedHealth + currentCard->GetStatModifierValue(CardStats::CARD_STAT_SPELL_DAMAGE) + card->GetStatModifierValue(CardStats::CARD_STAT_SPELL_DAMAGE_TAKEN), 0);
    if (applyDefense)
        damage = calculateReducedDamage(damage, card->GetModifiedDefense());

    card->DealDamage(damage);
    card->SubtractMana(drainedMana);

    restoredHealth = (std::max)(restoredHealth + currentCard->GetStatModifierValue(CardStats::CARD_STAT_SPELL_HEAL) + currentCard->GetStatModifierValue(CardStats::CARD_STAT_SPELL_DAMAGE_TAKEN), 0);
    currentCard->AddHealth(restoredHealth);
    currentCard->AddMana(restoredMana);

    Packet packet(SMSGPackets::SMSG_SPELL_DRAIN);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteBit(card->IsAlive());
    packet.FlushBits();

    packet << GetId();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet << damage << restoredHealth << drainedMana << restoredMana;
    packet << card->GetMana() << currentCard->GetHealth() << currentCard->GetMana();

    GetGame()->BroadcastPacket(packet);
}

// Ends player turn
void Player::endTurn()
{
    bool sendDeckCards = false;
    for (auto iter = m_currentCards.begin(); iter != m_currentCards.end();)
    {
        if ((*iter)->HasAuras())
        {
            (*iter)->HandleTickOnAuras();
            if (!(*iter)->IsAlive())
            {
                if (!sendDeckCards)
                    sendDeckCards = true;

                if (m_currentCardIndex >= iter - m_currentCards.begin())
                    --m_currentCardIndex;

                iter = m_currentCards.erase(iter);
                continue;
            }
        }

        ++iter;
    }

    if (sendDeckCards || ((m_currentCards.size() < MAX_CARDS_ON_DECK) && !m_cardOrder.empty()))
        HandleDeckCards(true);

    if (m_currentCards.empty())
    {
        EndGame(GetOpponent() ? GetOpponent()->GetId() : 0);
        return;
    }

    replenishMana();
    ++m_currentCardIndex;
    GetGame()->ActivateSecondPlayer();
}

bool Player::SwapCards(PlayableCard* card, PlayableCard* other) const
{
    static auto getCardIndex = [](std::vector<PlayableCard*> const& all, PlayableCard const* toFind)
    {
        auto const& cardIter = std::find(all.begin(), all.end(), toFind);
        return cardIter != all.end() ? (int)std::distance(all.begin(), cardIter) : -1;
    };

    auto& cards = card->GetOwner()->m_currentCards;
    auto& otherCards = card->GetOwner()->m_currentCards;

    int cardIdx = getCardIndex(cards, card);
    int otherIdx = getCardIndex(otherCards, other);
    bool success = cardIdx != -1 && otherIdx != -1;

    Packet packet(SMSGPackets::SMSG_SWAP_CARDS);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteByteStreamInOrder(card->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteBitStreamInOrder(other->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteByteStreamInOrder(other->GetGuid(), { 0, 1, 2, 3, 4, 5, 6, 7 });
    packet.WriteBit(success);
    packet.FlushBits();

    if (!success)
    {
        SendPacket(packet);
        return false;
    }

    std::swap(cards[cardIdx], otherCards[otherIdx]);
    Player* player = card->GetOwner();
    card->SetOwner(other->GetOwner());
    other->SetOwner(player);

    GetGame()->BroadcastPacket(packet);
    return true;
}
