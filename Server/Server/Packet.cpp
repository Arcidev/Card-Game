#include <cstring>
#include <stdexcept>
#include "Packet.h"

// Inicializes writeable packet
Packet::Packet(uint32_t opcodeNumber) : m_rpos(0), m_wpos(0), m_bitpos(8), m_curbitval(0)
{
    append<uint16_t>(opcodeNumber);
}

// Inicializes readable packet
Packet::Packet(std::string const& data) : m_rpos(0), m_wpos(0), m_bitpos(8), m_curbitval(0)
{
    append((uint8_t const*)data.c_str(), data.size());
}

// Inicializes data in packet
void Packet::Initialize(uint32_t opcodeNumber)
{
    m_wpos = 0;
    m_bitpos = 8;
    m_curbitval = 0;
    m_storage.clear();
    append<uint16_t>(opcodeNumber);
}

// Adds all bits stored in curbitval into stream as byte
void Packet::FlushBits()
{
    if (m_bitpos == 8)
        return;

    append((uint8_t *)&m_curbitval, sizeof(uint8_t));
    m_curbitval = 0;
    m_bitpos = 8;
}

// Appends data into packet stream
void Packet::append(uint8_t const* src, size_t cnt)
{
    if (m_storage.size() < m_wpos + cnt)
        m_storage.resize(m_wpos + cnt);

    std::memcpy(&m_storage[m_wpos], src, cnt);
    m_wpos += cnt;
}

// Writes one bit into curbitval
bool Packet::WriteBit(uint32_t bit)
{
    --m_bitpos;
    if (bit)
        m_curbitval |= (1 << (m_bitpos));

    if (m_bitpos == 0)
    {
        m_bitpos = 8;
        append((uint8_t*)&m_curbitval, sizeof(m_curbitval));
        m_curbitval = 0;
    }

    return (bit != 0);
}

// Appends specified type as byte values
template <typename T>
void Packet::append(T value)
{
    FlushBits();
    append((uint8_t *)&value, sizeof(value));
}

// Gets specified type from byte values
template <typename T>
T Packet::read(size_t pos)
{
    if (pos + sizeof(T) > m_storage.size())
        throw std::out_of_range("Reading out of Range");

    m_rpos += sizeof(T);
    return *((T const*)&m_storage[pos]);
}

// Writes byte value of guid if byte has value. If not its ignored
void Packet::WriteGuidByte(uint8_t byte)
{
    if (byte)
        append<uint8_t>(byte);
}

// Writes if exist byte values of guid in passed order
void Packet::WriteGuidBitStreamInOrder(Guid guid, std::vector<uint8_t> indexOrder)
{
    uint8_t size = (indexOrder.size() > 8) ? 8 : indexOrder.size();
    for (uint8_t i = 0; i < size; i++)
    {
        if (indexOrder[i] > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + indexOrder[i]);

        WriteBit(guid[indexOrder[i]]);
    }
}

// Writes byte values of guid if bytes has value. Bytes without value are ignored
void Packet::WriteGuidByteStreamInOrder(Guid guid, std::vector<uint8_t> indexOrder)
{
    uint8_t size = (indexOrder.size() > 8) ? 8 : indexOrder.size();
    for (uint8_t i = 0; i < size; i++)
    {
        if (indexOrder[i] > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + indexOrder[i]);

        WriteGuidByte(guid[indexOrder[i]]);
    }
}

// Reads 1 bit from stream
bool Packet::ReadBit()
{
    ++m_bitpos;
    if (m_bitpos > 7)
    {
        m_bitpos = 0;
        m_curbitval = read<uint8_t>(m_rpos);
    }

    return ((m_curbitval >> (7 - m_bitpos)) & 1) != 0;
}

// Stores uint32 value in stream
Packet& Packet::operator << (uint32_t value)
{
    append<uint32_t>(value);
    return *this;
}

// Stores uint16 value in stream
Packet& Packet::operator << (uint16_t value)
{
    append<uint16_t>(value);
    return *this;
}

// Stores uint8 value in stream
Packet& Packet::operator << (uint8_t value)
{
    append<uint8_t>(value);
    return *this;
}

// Stores float value in stream
Packet& Packet::operator << (float value)
{
    append<float>(value);
    return *this;
}

// Stores string and string length in stream
Packet& Packet::operator << (std::string value)
{
    *this << (uint16_t)value.size();
    append((uint8_t*)value.c_str(), value.size());
    return *this;
}

// Reads uint32 value from stream
Packet& Packet::operator >> (uint32_t &value)
{
    value = read<uint32_t>(m_rpos);
    return *this;
}

// Reads uint16 value from stream
Packet& Packet::operator >> (uint16_t &value)
{
    value = read<uint16_t>(m_rpos);
    return *this;
}

// Reads uint8 value from stream
Packet& Packet::operator >> (uint8_t &value)
{
    value = read<uint8_t>(m_rpos);
    return *this;
}

// Reads string and string length from stream
Packet& Packet::operator >> (std::string& value)
{
    uint16_t length;
    *this >> length;

    if (m_rpos + length > m_storage.size())
        throw std::out_of_range("Reading out of Range");

    value.clear();
    value.reserve(length);
    for (uint16_t i = 0; i < length; i++)
        value += m_storage[m_rpos + i];
    m_rpos += length;

    return *this;
}
