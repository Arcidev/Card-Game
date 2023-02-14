#pragma once
#include <cstdint>

class ConnectedUser;
class PlayableCard;
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
        void DisconnectPlayer(uint32_t playerId) const;
        void AddPlayer(Player* player);
        void ActivateSecondPlayer();
        void RemovePlayer(uint32_t playerId);
        void SendActivePlayer(PlayableCard const* card) const;

        uint64_t GetNextCardGuid() { return m_nextCardGuid++; };
        Player* GetOpponent(Player const* player) const;
        Player* GetActivePlayer() const;
        uint32_t GetActivePlayerId() const { return m_activePlayerId; }
        void BroadcastPacket(Packet const& packet) const;
};

