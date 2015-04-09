#pragma once
#include <cstdint>

class Guid
{
private:
    uint8_t m_byte[8];

    void setByteValues(uint64_t guid);
    uint64_t getGuidValue() const;

public:
    Guid(uint64_t guid);

    uint8_t& operator [] (uint8_t index);
    Guid& operator = (uint64_t guid);
    operator uint64_t() const { return getGuidValue(); };
};
