#include <cstring>
#include <stdexcept>
#include "ByteBuffer.h"

ByteBuffer::ByteBuffer() : m_rpos(0), m_wpos(0), m_bitpos(8), m_curbitval(0) { }

// Adds all bits stored in curbitval into stream as byte
void ByteBuffer::FlushBits()
{
    if (m_bitpos == 8)
        return;

    append((uint8_t *)&m_curbitval, sizeof(uint8_t));
    m_curbitval = 0;
    m_bitpos = 8;
}

// Appends data into ByteBuffer stream
void ByteBuffer::append(uint8_t const* src, size_t cnt)
{
    if (m_storage.size() < m_wpos + cnt)
        m_storage.resize(m_wpos + cnt);

    std::memcpy(&m_storage[m_wpos], src, cnt);
    m_wpos += cnt;
}

// Writes one bit into curbitval
bool ByteBuffer::WriteBit(uint32_t const& bit)
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
void ByteBuffer::append(T const& value)
{
    FlushBits();
    append((uint8_t *)&value, sizeof(value));
}

// Gets specified type from byte values
template <typename T>
T ByteBuffer::read(size_t const& pos)
{
    if (pos + sizeof(T) > m_storage.size())
        throw std::out_of_range("Reading out of Range");

    T val = *((T const*)&m_storage[pos]);
    m_rpos += sizeof(T);
    return val;
}

// Writes byte value of guid if byte has value. If not its ignored
void ByteBuffer::WriteByte(uint8_t const& byte)
{
    if (byte)
        append<uint8_t>(byte);
}

// Reads byte value of guid if byte has value. If not its ignored
void ByteBuffer::ReadByte(uint8_t& byte)
{
    if (byte)
        byte = read<uint8_t>(m_rpos);
}

// Writes if exist byte value of guid in passed order
void ByteBuffer::WriteBitStreamInOrder(Guid const& guid, std::initializer_list<uint8_t> const& indexOrder)
{
    for (uint8_t const& val : indexOrder)
    {
        if (val > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + val);

        WriteBit(guid[val]);
    }
}

// Writes byte values of guid if bytes has value. Bytes without value are ignored
void ByteBuffer::WriteByteStreamInOrder(Guid const& guid, std::initializer_list<uint8_t> const& indexOrder)
{
    for (uint8_t const& val : indexOrder)
    {
        if (val > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + val);

        WriteByte(guid[val]);
    }
}

// Reads if exist byte values of guid in passed order
void ByteBuffer::ReadBitStreamInOrder(Guid& guid, std::initializer_list<uint8_t> const& indexOrder)
{
    for (uint8_t const& val : indexOrder)
    {
        if (val > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + val);

        guid[val] = ReadBit();
    }
}

// Reads byte values of guid if bytes has value. Bytes without value are ignored
void ByteBuffer::ReadByteStreamInOrder(Guid& guid, std::initializer_list<uint8_t> const& indexOrder)
{
    for (uint8_t const& val : indexOrder)
    {
        if (val > 7)
            throw std::out_of_range("Index must be lower than 8. You've requested " + val);

        ReadByte(guid[val]);
    }
}

// Reads 1 bit from stream
bool ByteBuffer::ReadBit()
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
ByteBuffer& ByteBuffer::operator << (uint32_t const& value)
{
    append<uint32_t>(value);
    return *this;
}

// Stores uint16 value in stream
ByteBuffer& ByteBuffer::operator << (uint16_t const& value)
{
    append<uint16_t>(value);
    return *this;
}

// Stores uint8 value in stream
ByteBuffer& ByteBuffer::operator << (uint8_t const& value)
{
    append<uint8_t>(value);
    return *this;
}

// Stores float value in stream
ByteBuffer& ByteBuffer::operator << (float const& value)
{
    append<float>(value);
    return *this;
}

// Stores string and string length in stream
ByteBuffer& ByteBuffer::operator << (std::string const& value)
{
    *this << (uint16_t)value.size();
    append((uint8_t const*)value.c_str(), value.size());
    return *this;
}

// Stores vector and vector length in stream
ByteBuffer& ByteBuffer::operator << (std::vector<uint8_t> const& value)
{
    *this << (uint16_t)value.size();
    append(value.data(), value.size());
    return *this;
}


// Stores stream from another buffer in this buffer
ByteBuffer& ByteBuffer::operator << (ByteBuffer const& value)
{
    if (value.GetStorage().empty())
        return *this;

    append(value.GetStorage().data(), value.GetStorage().size());
    return *this;
}

// Reads uint32 value from stream
ByteBuffer& ByteBuffer::operator >> (uint32_t &value)
{
    value = read<uint32_t>(m_rpos);
    return *this;
}

// Reads uint16 value from stream
ByteBuffer& ByteBuffer::operator >> (uint16_t &value)
{
    value = read<uint16_t>(m_rpos);
    return *this;
}

// Reads uint8 value from stream
ByteBuffer& ByteBuffer::operator >> (uint8_t &value)
{
    value = read<uint8_t>(m_rpos);
    return *this;
}

// Reads string and string length from stream
ByteBuffer& ByteBuffer::operator >> (std::string& value)
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

// Reads vector and vector length from stream
ByteBuffer& ByteBuffer::operator >> (std::vector<uint8_t>& value)
{
    uint16_t length;
    *this >> length;

    if (m_rpos + length > m_storage.size())
        throw std::out_of_range("Reading out of Range");

    value.resize(length);
    for (uint16_t i = 0; i < length; i++)
        value[i] = m_storage[m_rpos + i];

    m_rpos += length;
    return *this;
}
