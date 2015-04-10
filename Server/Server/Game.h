#pragma once
#include <cstdint>

class Player;
class Packet;

class Game
{
    private:
        uint32_t m_activePlayerId;
        Player* m_player1;
        Player* m_player2;
        uint64_t m_nextCardGuid;

    public:
        Game();
        ~Game();

        bool IsFull() const { return m_player1 && m_player2; }
        bool IsEmpty() const;
        void DisconnectPlayer(uint32_t playerID);
        void AddPlayer(Player* player);
        void ActivateSecondPlayer();

        uint64_t GetNextCardGuid() { return m_nextCardGuid++; };
        Player* GetOpponent(Player const* player) const;
        Player* GetActivePlayer() const;
        uint32_t GetActivePlayerId() const { return m_activePlayerId; }
        void BroadcastPacket(Packet const* packet) const;
};

