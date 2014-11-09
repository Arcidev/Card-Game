#pragma once

typedef unsigned int uint;

class Player;
class Packet;

class Game
{
    private:
        Player* m_player1;
        Player* m_player2;

    public:
        Game();
        ~Game();

        bool IsFull() const { return m_player2 != nullptr; } // player1 must be always there
        void RemovePlayer(uint playerID);
        void AddPlayer(Player* player);

        Player* GetOpponent(Player const* player) const;
        void BroadcastPacket(Packet const* packet) const;
};

