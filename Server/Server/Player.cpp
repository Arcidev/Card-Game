#include <stdexcept>
#include <algorithm>
#include <random>
#include "Player.h"
#include "serverNetwork.h"
#include "DataHolder.h"
#include "PacketHandlers/PacketHandler.h"
#include "Cards/PlayableCard.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuraEffect.h"
#include "../Crypto/Aes.h"
#include "PacketHandlers/Packet.h"
#include "PlayerDefines.h"
#include "StaticHelper.h"
#include "../Shared/SharedDefines.h"

Player::Player(uint32_t id, SOCKET socket, Game* game, ServerNetwork* network) : m_isPrepared(false), m_isDisconnected(false), m_id(id), m_currentCardIndex(0), m_game(game), m_network(network), m_socket(socket), m_name("<unknown>"){}

Player::~Player()
{
    for (CardsMap::iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
        delete iter->second;
}

// Set player state to disconnected
void Player::Disconnect()
{
    m_isDisconnected = true;
    m_game->DisconnectPlayer(m_id);
    m_network->OnPlayerDisconnected(this);
    shutdown(m_socket, SD_BOTH);
    DEBUG_LOG("Client %d: Connection closed\r\n", m_id);
}

// Sends attack result
void Player::SendAttackResult(uint8_t const& result, uint64_t const& cardGuid, uint8_t const& damage) const
{
    Packet packet(SMSG_ATTACK_RESULT);
    packet << result;
    if (result)
    {
        packet.WriteGuidBitStreamInOrder(cardGuid, std::vector<uint8_t> { 6, 2, 1, 7, 3, 0, 4, 5 });
        packet.FlushBits();
        packet.WriteGuidByteStreamInOrder(cardGuid, std::vector<uint8_t> { 2, 6, 7 });
        packet << m_id;
        packet.WriteGuidByteStreamInOrder(cardGuid, std::vector<uint8_t> { 1, 3, 0 });
        packet << damage;
        packet.WriteGuidByteStreamInOrder(cardGuid, std::vector<uint8_t> { 5, 4 });
        GetGame()->BroadcastPacket(&packet);
    }
    else
        SendPacket(&packet);
}

// Attacks enemy player
void Player::Attack(uint64_t const& victimCardGuid)
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
        SendAttackResult(ATTACK_RESULT_INVALID_TARGET, 0, 0);
        return;
    }

    uint8_t damage = currentCard->GetDamage();
    float reduction = (float)(victimCard->GetModifiedDefense() * DEFENSE_PERCENT_PER_POINT);
    if (reduction)
    {
        reduction /= 100.0f;
        reduction += 1.0f;
        damage = (uint8_t)(damage / reduction);
    }

    victimCard->DealDamage(damage);

    if (!victimCard->IsAlive())
    {
        victim->destroyCard(victimCardGuid);
        SendAttackResult(ATTACK_RESULT_CARD_DESTROYED, victimCardGuid, damage);
        if (victim->m_cardOrder.empty() && victim->m_currentCards.empty())
            SendEndGame(m_id);
        else
            victim->HandleDeckCards(victim->m_currentCards.empty() ? true : false);
    }
    else
        SendAttackResult(ATTACK_RESULT_CARD_ATTACKED, victimCardGuid, damage);

    endTurn();
}

// Sends information about failed spell cast to client
void Player::SendSpellCastFailed(uint8_t const& reason) const
{
    Packet packet(SMSG_SPELL_CAST_FAILED);
    packet << reason;
    SendPacket(&packet);
}

// Uses card spell
void Player::UseSpell(uint64_t const& selectedCardGuid)
{
    if (!IsActive())
        return;

    PlayableCard* currentCard = GetCurrentCard();
    Player* victim = GetGame()->GetOpponent(this);
    if (!victim || !currentCard)
        return;

    if (!currentCard->GetSpell())
    {
        SendSpellCastFailed(SPELL_FAIL_CANT_CAST_SPELLS);
        return;
    }

    uint8_t failedCastReason = currentCard->GetSpell()->Cast(this, victim, selectedCardGuid);
    if (failedCastReason)
    {
        SendSpellCastFailed(failedCastReason);
        return;
    }

    endTurn();
}

// Removes card from player deck
void Player::destroyCard(uint64_t const& cardGuid)
{
    for (uint32_t i = 0; i < m_currentCards.size(); ++i)
    {
        if (m_currentCards[i]->GetGuid() == cardGuid)
        {
            m_currentCards.erase(m_currentCards.begin() + i);

            if (i >= m_currentCardIndex)
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

// Receive encrypted packet from client
void Player::ReceivePacket(uint32_t const& dataLength, char const* data)
{
    uint32_t readedData = 0;
    
    while (readedData < dataLength)
    {
        uint16_t packetLength;
        std::memcpy(&packetLength, data + readedData, sizeof(uint16_t));
        std::string networkData;
        networkData.resize(packetLength);
        std::memcpy(&networkData[0], data + sizeof(uint16_t) + readedData, packetLength);

        // Inicializes readable packet with decrypted data
        Packet packet(Aes::Decrypt(networkData, m_AesKey));
        try
        {
            uint16_t packetType;
            packet >> packetType;

            PacketHandlerFunc packetHandler = PacketHandler::GetPacketHandler(packetType);
            if (packetHandler)
                packetHandler(this, &packet);
        }
        catch (std::out_of_range ex)
        {
            printf("Error occured while reading packet: %s\n", ex.what());
        }

        readedData += dataLength + sizeof(uint16_t);
    }
}

// Sends all cards that are currently available to be played with
void Player::SendAvailableCards() const
{
    // can be static, always sending the same
    CardsDataMap cards = DataHolder::GetCards();
    Packet packet(SMSG_AVAILABLE_CARDS);
    packet << (uint16_t)cards.size();

    ByteBuffer buffer;
    for (CardsDataMap::const_iterator iter = cards.begin(); iter != cards.end(); ++iter)
    {
        Spell const* spell = iter->second.GetSpell();
        packet.WriteBit(spell ? true : false);

        buffer << iter->second.GetId();
        buffer << iter->second.GetType();
        buffer << iter->second.GetHealth();

        if (spell)
        {
            buffer << spell->GetManaCost();
            buffer << spell->GetId();
            buffer << (uint8_t)spell->GetSpellEffects().size();
            for (std::list<SpellEffectPair>::const_iterator iter = spell->GetSpellEffects().begin(); iter != spell->GetSpellEffects().end(); ++iter)
            {

                buffer << iter->second.Target;
                buffer << iter->second.Value1;
                buffer << iter->second.Value2;
                buffer << iter->second.Value3;
                buffer << iter->second.Value4;
            }
        }

        buffer << iter->second.GetDamage();
        buffer << iter->second.GetMana();
        buffer << iter->second.GetDefense();
        buffer << iter->second.GetPrice();
    }

    packet.FlushBits();
    packet.AppendBuffer(buffer);

    SendPacket(&packet);
}

// Sends whisper response to sender
void Player::SendChatWhisperResponse(std::string const& message, std::string const& receiver, bool success) const
{
    Packet pck(success ? SMSG_CHAT_MESSAGE : SMSG_WHISPER_FAILED);

    if (success)
    {
        pck << (uint8_t)CHAT_WHISPER_RESPONSE;
        pck << receiver;
        pck << message;
    }
    else
        pck << receiver;
    
    SendPacket(&pck);
}

// Sends selection card has failed
void Player::SendSelectCardsFailed(uint8_t const& failReason) const
{
    Packet packet(SMSG_SELECT_CARDS_FAILED);
    packet << failReason;

    SendPacket(&packet);
}

// Sends info about opponent if is already connected
void Player::SendInitResponse() const
{
    Packet pck(SMSG_INIT_RESPONSE);
    pck.WriteBit(m_game->IsFull());
    pck.FlushBits();

    pck << m_id;
    if (m_game->IsFull())
    {
        pck << GetOpponent()->GetId();
        pck << GetOpponent()->GetName();
    }

    SendPacket(&pck);
}

// Sends information about disconnected opponent
void Player::SendPlayerDisconnected() const
{
    Packet packet(SMSG_PLAYER_DISCONNECTED);
    SendPacket(&packet);
}

// Sends encrypted packet to client
void Player::SendPacket(Packet const* packet) const
{
    std::string encrypted = Aes::Encrypt(std::string(packet->GetStorage().begin(), packet->GetStorage().end()), m_AesKey);
    uint16_t size = encrypted.length();
    std::vector<uint8_t> toSend(sizeof(uint16_t) + size);
    std::memcpy(&toSend[0], (uint8_t *)&size, sizeof(uint16_t));
    std::memcpy(&toSend[0] + sizeof(uint16_t), encrypted.c_str(), size);

    NetworkServices::sendMessage(m_socket, &toSend[0], toSend.size());
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
    if (addCard && (!m_cardOrder.empty() || (m_currentCards.size() < MAX_CARDS_ON_DECK)))
    {
        m_currentCards.push_back(m_cardOrder.front());
        m_cardOrder.erase(m_cardOrder.begin());
    }
    
    uint8_t cardsCount = m_currentCards.size();
    Packet packet(SMSG_DECK_CARDS);
    packet << cardsCount;
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteGuidBitStreamInOrder(m_currentCards[i]->GetGuid(), std::vector<uint8_t> { 7, 2, 1, 4, 5, 0, 6, 3 });
    packet.FlushBits();

    packet << m_id;
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteGuidByteStreamInOrder(m_currentCards[i]->GetGuid(), std::vector<uint8_t> { 2, 1, 7, 6, 0, 5, 3, 4 });
    
    m_game->BroadcastPacket(&packet);
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
void Player::SendEndGame(uint32_t const& winnerId) const
{
    Packet packet(SMSG_END_GAME);
    packet << winnerId;

    GetGame()->BroadcastPacket(&packet);
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
void Player::SendCardStatChanged(PlayableCard const* card, uint8_t const& cardStat) const
{
    Packet packet(SMSG_CARD_STAT_CHANGED);
    packet.WriteGuidBitStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 2, 6, 7, 1, 0, 3, 5, 4 });
    packet.FlushBits();

    packet.WriteGuidByteStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 5, 7 });
    packet << card->GetStatModifierValue(cardStat);
    packet.WriteGuidByteStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 6, 3, 1 });
    packet << m_id;
    packet.WriteGuidByteStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 0, 4, 2 });
    packet << cardStat;

    m_game->BroadcastPacket(&packet);
}

void Player::SendApplyAura(uint64_t const& targetGuid, SpellAuraEffect const* aura) const
{
    Packet packet(SMSG_APPLY_AURA);
    packet.WriteGuidBitStreamInOrder(targetGuid, std::vector<uint8_t> { 7, 2, 1, 3, 5, 4, 0, 6 });
    packet.FlushBits();

    packet << m_id;
    packet.WriteGuidByteStreamInOrder(targetGuid, std::vector < uint8_t > { 0, 5, 2, 1, 7, 6, 4, 3 });
    packet << aura->GetSpellId();

    GetGame()->BroadcastPacket(&packet);
}

// Ends player turn
void Player::endTurn()
{
    if ((m_currentCards.size() < MAX_CARDS_ON_DECK) && !m_cardOrder.empty())
        HandleDeckCards(true);

    ++m_currentCardIndex;
    GetGame()->ActivateSecondPlayer();
}

void Player::DealPeriodicDamage(PlayableCard* card, uint32_t const& damage)
{
    card->DealDamage(damage);

    Packet packet(SMSG_SPELL_PERIODIC_DAMAGE);
    packet.WriteGuidBitStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 6, 4, 1 });
    packet.WriteBit(card->IsAlive());
    packet.WriteGuidBitStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 7, 2, 3, 5, 0 });
    packet.FlushBits();

    packet << m_id;
    packet.WriteGuidByteStreamInOrder(card->GetGuid(), std::vector<uint8_t> { 0, 1, 2, 3, 7, 5, 4, 6 });
    packet << damage;
    GetGame()->BroadcastPacket(&packet);

    if (!card->IsAlive())
    {
        destroyCard(card->GetGuid());

        if (m_cardOrder.empty() && m_currentCards.empty())
            SendEndGame(GetOpponent() ? GetOpponent()->GetId() : 0);
        else
            HandleDeckCards(m_currentCards.empty() ? true : false);
    }
}
