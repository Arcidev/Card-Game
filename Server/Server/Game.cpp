#include "Game.h"
#include "Player.h"
#include "Packet.h"

Game::Game() : m_activePlayerId(0), m_player1(nullptr), m_player2(nullptr), m_nextCardGuid(1) {}

Game::~Game()
{
    if (m_player1)
        delete m_player1;

    if (m_player2)
        delete m_player2;
}

void Game::DisconnectPlayer(uint32_t playerID)
{
    Player* opponent = nullptr;
    
    if (m_player1 && (playerID == m_player1->GetId()))
        opponent = m_player2;
    else if (m_player2 && (playerID == m_player2->GetId()))
        opponent = m_player1;

    if (opponent && !opponent->IsDisconnected())
        opponent->SendPlayerDisconnected();
}

bool Game::IsEmpty() const
{
    return (!m_player1 || m_player1->IsDisconnected()) && (!m_player2 || m_player2->IsDisconnected());
}

void Game::AddPlayer(Player* player)
{
    if (!m_player1)
        m_player1 = player;
    else if (!m_player2)
        m_player2 = player;
}

Player* Game::GetOpponent(Player const* player) const
{
    return player->GetId() == m_player1->GetId() ? m_player2 : m_player1;
}

Player* Game::GetActivePlayer() const
{
    return m_activePlayerId == m_player1->GetId() ? m_player1 : m_player2;
}

void Game::BroadcastPacket(Packet const* packet) const
{
    if (m_player1)
        m_player1->SendPacket(packet);

    if (m_player2)
        m_player2->SendPacket(packet);
}

void Game::ActivateSecondPlayer()
{
    if (m_activePlayerId)
        m_activePlayerId = (m_player1->GetId() == m_activePlayerId) ? m_player2->GetId() : m_player1->GetId();
    else
        m_activePlayerId = (rand() % 2) ? m_player1->GetId() : m_player2->GetId();

    Card* currentCard = GetActivePlayer()->GetCurrentCard();
    if (!currentCard)
        return;

    Packet packet(SMSG_ACTIVE_PLAYER);
    packet.WriteGuidBitStreamInOrder(currentCard->GetGuid(), std::vector<uint8_t> { 7, 1, 2, 0, 3, 5, 4, 6 });
    packet.FlushBits();

    packet.WriteGuidByteStreamInOrder(currentCard->GetGuid(), std::vector<uint8_t> { 7, 5, 4, 2, 6 });
    packet << m_activePlayerId;
    packet.WriteGuidByteStreamInOrder(currentCard->GetGuid(), std::vector<uint8_t> { 1, 0, 3 });

    BroadcastPacket(&packet);
}
