#include <stdexcept>
#include "Guid.h"

// Inicializes new instance of Guid
Guid::Guid()
{
    setByteValues(0);
}

// Inicialize Guid object from uint64_t value
Guid::Guid(uint64_t guid)
{
    setByteValues(guid);
}

// Sets Uint64 value as aray of 8 byte values
void Guid::setByteValues(uint64_t guid)
{
    for (uint8_t i = 0; i < 8; i++)
        m_byte[i] = ((uint8_t *)(&guid))[i];
}

// Allows to use class as array
uint8_t& Guid::operator[](uint8_t index)
{
    if (index > 7)
        throw std::out_of_range("Index must be lower than 8. You've requested " + index);

    return m_byte[index];
}

// Allows to use class as array
uint8_t const& Guid::operator[](uint8_t index) const
{
    if (index > 7)
        throw std::out_of_range("Index must be lower than 8. You've requested " + index);

    return m_byte[index];
}

// Allows to set uint64 value at any time
Guid& Guid::operator = (uint64_t guid)
{
    setByteValues(guid);
    return *this;
}

// Returns array of byte values as uint64 value
uint64_t Guid::getGuidValue() const
{
    uint64_t guid = 0;
    for (uint8_t i = 0; i < 8; i++)
        guid = (guid << 8) | m_byte[7 - i];

    return guid;
}
