#pragma once
#include <string>
#include <vector>
#include "Guid.h"

class ByteBuffer
{
    protected:
        size_t m_rpos, m_wpos, m_bitpos;
        uint8_t m_curbitval;
        std::vector<uint8_t> m_storage;

        void append(uint8_t const* src, size_t cnt);
        template <typename T> void append(T value);
        template <typename T> T read(size_t pos);

    public:
        ByteBuffer();
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

        ByteBuffer& operator << (uint32_t const& value);
        ByteBuffer& operator << (int32_t const& value) { return operator<<((uint32_t)value); }
        ByteBuffer& operator << (uint16_t const& value);
        ByteBuffer& operator << (int16_t const& value) { return operator<<((uint16_t)value); }
        ByteBuffer& operator << (uint8_t const& value);
        ByteBuffer& operator << (int8_t const& value) { return operator<<((uint8_t)value); }
        ByteBuffer& operator << (float const& value);
        ByteBuffer& operator << (std::string const& value);

        ByteBuffer& operator >> (uint32_t& value);
        ByteBuffer& operator >> (uint16_t& value);
        ByteBuffer& operator >> (uint8_t& value);
        ByteBuffer& operator >> (std::string& value);
};
