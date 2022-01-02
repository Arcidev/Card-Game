#include "ConnectedUser.h"
#include "ServerNetwork.h"
#include "Friend.h"
#include "PacketHandlers/Packet.h"

Friend::Friend(uint32_t userId, std::string_view userName, ServerNetwork const* network) : m_userId(userId), m_userName(userName), m_network(network) { }

bool Friend::IsOnline() const
{
    ConnectedUser const* user = getUser();
    return user && !user->IsDisconnected();
}

void Friend::SendOwnersOnlineStatus(std::string_view name, bool isOnline) const
{
    if (ConnectedUser const* user = getUser())
    {
        Packet packet(SMSGPackets::SMSG_FRIEND_STATUS);
        packet.WriteBit(isOnline);
        packet.FlushBits();
        packet << name;

        user->SendPacket(packet);
    }
}

ConnectedUser const* Friend::getUser() const
{
    DbUserMap::const_iterator iter = m_network->GetDbUsers().find(GetId());
    return iter != m_network->GetDbUsers().end() ? iter->second : nullptr;
}
