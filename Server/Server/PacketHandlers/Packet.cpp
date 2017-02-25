#include "Packet.h"

// Inicializes writeable packet
Packet::Packet(uint32_t opcodeNumber)
{
    append<uint16_t>(opcodeNumber);
}

// Inicializes readable packet
Packet::Packet(std::vector<uint8_t> const& data)
{
    append(data.data(), data.size());
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
