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
        void WriteByte(uint8_t b);
        void ReadByte(uint8_t& byte);
        void WriteBitStreamInOrder(Guid const& guid, std::initializer_list<uint8_t> const& indexOrder);
        void WriteByteStreamInOrder(Guid const& guid, std::initializer_list<uint8_t> const& indexOrder);
        void ReadBitStreamInOrder(Guid& guid, std::initializer_list<uint8_t> const& indexOrder);
        void ReadByteStreamInOrder(Guid& guid, std::initializer_list<uint8_t> const& indexOrder);

        bool ReadBit();
        size_t GetReadPosition() const { return m_rpos; }

        std::vector<uint8_t> const& GetStorage() const { return m_storage; }

        ByteBuffer& operator << (uint64_t value);
        ByteBuffer& operator << (int64_t value) { return operator<<((uint64_t)value); }
        ByteBuffer& operator << (uint32_t value);
        ByteBuffer& operator << (int32_t value) { return operator<<((uint32_t)value); }
        ByteBuffer& operator << (uint16_t value);
        ByteBuffer& operator << (int16_t value) { return operator<<((uint16_t)value); }
        ByteBuffer& operator << (uint8_t value);
        ByteBuffer& operator << (int8_t value) { return operator<<((uint8_t)value); }
        ByteBuffer& operator << (float value);
        ByteBuffer& operator << (std::string_view value);
        ByteBuffer& operator << (std::vector<uint8_t> const& value);
        ByteBuffer& operator << (ByteBuffer const& value);

        ByteBuffer& operator >> (uint32_t& value);
        ByteBuffer& operator >> (uint16_t& value);
        ByteBuffer& operator >> (uint8_t& value);
        ByteBuffer& operator >> (std::string& value);
        ByteBuffer& operator >> (std::vector<uint8_t>& value);
};
