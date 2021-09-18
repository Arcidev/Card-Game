#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_GET_ACHIEVEMENTS packet
void PacketHandler::handleGetAchievementsPacket(ConnectedUser* cUser, Packet& /*packet*/)
{
    DEBUG_LOG("CMSG_GET_ACHIEVEMENTS:\r\n");
    auto const& achievements = cUser->GetAchievements();
    Packet result(SMSGPackets::SMSG_ACHIEVEMENTS);
    result << (uint16_t)achievements.size();

    for (auto const& achievement : achievements)
    {
        result << achievement.first;
        result << achievement.second.GetParentId();
        result.WriteBit(achievement.second.IsCompleted());
        result.FlushBits();

        auto const& criterias = achievement.second.GetCriterias();
        result << (uint8_t)criterias.size();
        for (auto const& criteria : criterias)
        {
            result << criteria.GetId();
            result.WriteBit(criteria.IsMet());
            result.FlushBits();

            if (!criteria.IsMet())
            {
                result << criteria.GetProgress();
                result << criteria.GetRequirement();
            }
        }
    }

    cUser->SendPacket(result);
}
