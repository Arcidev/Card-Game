#include "Game.h"
#include "ConnectedUser.h"
#include "Player.h"
#include "PacketHandlers/Packet.h"
#include "Cards/PlayableCard.h"

Game::Game() : m_activePlayer(nullptr), m_player1(nullptr), m_player2(nullptr), m_nextCardGuid(1) {}

// Free resources
Game::~Game()
{
    if (m_player1)
        delete m_player1;

    if (m_player2)
        delete m_player2;
}

// Called when player has been disconnected from server
void Game::DisconnectPlayer(uint32_t playerId) const
{
    Player const* opponent = nullptr;
    
    if (m_player1 && (playerId == m_player1->GetId()))
        opponent = m_player2;
    else if (m_player2 && (playerId == m_player2->GetId()))
        opponent = m_player1;

    // Informs opponent about disconnection
    if (opponent && !opponent->GetUser()->IsDisconnected())
        opponent->SendPlayerDisconnected();
}

// Removes player from game and destroys disconnected players
void Game::RemovePlayer(uint32_t playerId)
{
    Player const* opponnent = nullptr;
    if (m_player1 && (playerId == m_player1->GetId()))
    {
        m_player1 = nullptr;
        opponnent = m_player2;
    }
    else if (m_player2 && (playerId == m_player2->GetId()))
    {
        m_player2 = nullptr;
        opponnent = m_player1;
    }

    if (!opponnent || opponnent->GetUser()->IsDisconnected())
        delete this;
}

// Check if this game has players
bool Game::IsEmpty() const
{
    return (!m_player1 || m_player1->GetUser()->IsDisconnected()) && (!m_player2 || m_player2->GetUser()->IsDisconnected());
}

// Adds player to this game
void Game::AddPlayer(Player* player)
{
    if (!m_player1)
        m_player1 = player;
    else if (!m_player2)
        m_player2 = player;
}

// Returns player opponent
Player* Game::GetOpponent(Player const* player) const
{
    return player->GetId() == m_player1->GetId() ? m_player2 : m_player1;
}

// Sends packet to all players in this game
void Game::BroadcastPacket(Packet const& packet) const
{
    if (m_player1)
        m_player1->SendPacket(packet);

    if (m_player2)
        m_player2->SendPacket(packet);
}

// Activates second player and deactives the active one
void Game::ActivateSecondPlayer()
{
    if (m_activePlayer)
        m_activePlayer = (m_player1 == m_activePlayer) ? m_player2 : m_player1;
    else
        m_activePlayer = (rand() % 2) ? m_player1 : m_player2;

    PlayableCard* currentCard = m_activePlayer->GetCurrentCard();
    if (!currentCard)
        return;

    // Stop defending if is in defend state
    currentCard->SetDefendState(false);
    SendActivePlayer(currentCard);
}

void Game::SendActivePlayer(PlayableCard const* card) const
{
    Packet packet(SMSGPackets::SMSG_ACTIVE_PLAYER);
    packet.WriteBitStreamInOrder(card->GetGuid(), { 7, 1, 2, 0, 3, 5, 4, 6 });
    packet.FlushBits();

    packet.WriteByteStreamInOrder(card->GetGuid(), { 7, 5, 4, 2, 6 });
    packet << card->GetOwner()->GetId();
    packet.WriteByteStreamInOrder(card->GetGuid(), { 1, 0, 3 });

    BroadcastPacket(packet);
}
