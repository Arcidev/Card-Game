#include "Packet.h"

// Inicializes writeable packet
Packet::Packet(SMSGPackets opcodeNumber)
{
    append((uint16_t)opcodeNumber);
}

// Inicializes readable packet
Packet::Packet(std::vector<uint8_t> const& data)
{
    append(data.data(), data.size());
}

