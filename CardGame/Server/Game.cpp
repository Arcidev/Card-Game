#include "Game.h"
#include "Player.h"

Game::Game() : m_player1(nullptr), m_player2(nullptr) { }

Game::~Game()
{

}

void Game::RemovePlayer(uint playerID)
{
    if (m_player1 && (playerID == m_player1->GetID()))
        m_player1 = nullptr;
    else if (m_player2 && (playerID == m_player2->GetID()))
        m_player2 = nullptr;

    if (!m_player1 && !m_player2)
        delete this;
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
    return player->GetID() == m_player1->GetID() ? m_player2 : m_player1;
}

void Game::BroadcastPacket(Packet const* packet) const
{
    if (m_player1)
        m_player1->SendPacket(packet);

    if (m_player2)
        m_player2->SendPacket(packet);
}
