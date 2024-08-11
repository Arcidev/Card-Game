#pragma once
#include <cstdint>
#include <list>

class PlayableCard;
class Player;

class SpellTargetSelector
{
    private:
        typedef std::list<PlayableCard*>(*SpellTargetSelectorFunc)(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);

        static SpellTargetSelectorFunc const m_spellTargetSelectors[];

        // target handlers
        static std::list<PlayableCard*> handleTargetUnitTargetEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);
        static std::list<PlayableCard*> handleTargetUnitTargetFriend(Player const* attacker, Player const* /*victim*/, uint64_t targetGuid, uint32_t spellAttributes);
        static std::list<PlayableCard*> handleTargetUnitCleaveEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);
        static std::list<PlayableCard*> handleTargetUnitSelf(Player const* attacker, Player const* /*victim*/, uint64_t targetGuid, uint32_t /*spellAttributes*/);

    public:
        static std::list<PlayableCard*> GetTargets(uint8_t spellTarget, Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);
};
