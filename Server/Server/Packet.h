#pragma once
#include <string>
#include <vector>
#include "Guid.h"

#define MAX_PACKET_SIZE 10000

enum CMSGPackets
{
    CMSG_INIT_PACKET = 1,
    CMSG_CHAT_MESSAGE,
    CMSG_SELECTED_CARDS,
    CMSG_ATTACK_CARD,
    CMSG_DEFEND_SELF
};

enum SMSGPackets
{
    SMSG_INIT_RESPONSE = 1,
    SMSG_AVAILABLE_CARDS,
    SMSG_CHAT_MESSAGE,
    SMSG_WHISPER_FAILED,
    SMSG_SELECT_CARDS_FAILED,
    SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER,
    SMSG_SELECT_CARDS,
    SMSG_DECK_CARDS,
    SMSG_ACTIVE_PLAYER,
    SMSG_PLAYER_DISCONNECTED,
    SMSG_ATTACK_RESULT,
    SMSG_END_GAME,
    SMSG_CARD_STAT_CHANGED,
    SMSG_SPELL_CAST_FAILED,
    SMSG_SPELL_DAMAGE,
    SMSG_APPLY_AURA,
    SMSG_SPELL_PERIODIC_DAMAGE
};

class Packet
{
    private:
        size_t m_rpos, m_wpos, m_bitpos;
        uint8_t m_curbitval;
        std::vector<uint8_t> m_storage;

        void append(uint8_t const* src, size_t cnt);
        template <typename T> void append(T value);
        template <typename T> T read(size_t pos);

    public:
        Packet(uint32_t opcodeNumber);
        Packet(std::string const& data);
        void Initialize(uint32_t opcodeNumber);
        void FlushBits();
        bool WriteBit(uint32_t bit);
        void WriteGuidByte(uint8_t b);
        void ReadGuidByte(uint8_t& byte);
        void WriteGuidBitStreamInOrder(Guid const& guid, std::vector<uint8_t> indexOrder);
        void WriteGuidByteStreamInOrder(Guid const& guid, std::vector<uint8_t> indexOrder);
        void ReadGuidBitStreamInOrder(Guid& guid, std::vector<uint8_t> indexOrder);
        void ReadGuidByteStreamInOrder(Guid& guid, std::vector<uint8_t> indexOrder);

        bool ReadBit();
        size_t GetReadPosition() const { return m_rpos; }

        std::vector<uint8_t> const& GetStorage() const { return m_storage; }

        Packet& operator << (uint32_t const& value);
        Packet& operator << (int32_t const& value) { return operator<<((uint32_t)value); }
        Packet& operator << (uint16_t const& value);
        Packet& operator << (int16_t const& value) { return operator<<((uint16_t)value); }
        Packet& operator << (uint8_t const& value);
        Packet& operator << (int8_t const& value) { return operator<<((uint8_t)value); }
        Packet& operator << (float const& value);
        Packet& operator << (std::string const& value);

        Packet& operator >> (uint32_t& value);
        Packet& operator >> (uint16_t& value);
        Packet& operator >> (uint8_t& value);
        Packet& operator >> (std::string& value);
};
