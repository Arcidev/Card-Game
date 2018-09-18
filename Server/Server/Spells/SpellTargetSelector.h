#pragma once
#include <cstdint>
#include <list>

enum SpellTargets
{
    TARGET_UNIT_TARGET_ENEMY = 0,
    TARGET_UNIT_TARGET_FRIEND,
    TARGET_UNIT_CLEAVE_ENEMY,
    MAX_SPELL_EFFECT_TARGET
};

class PlayableCard;
class Player;
typedef std::list<PlayableCard*>(*SpellTargetSelectorFunc)(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);

class SpellTargetSelector
{
    private:
        static SpellTargetSelectorFunc const m_spellTargetSelectors[];

        // target handlers
        static std::list<PlayableCard*> handleTargetUnitTargetEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);
        static std::list<PlayableCard*> handleTargetUnitTargetFriend(Player const* attacker, Player const* /*victim*/, uint64_t targetGuid, uint32_t spellAttributes);
        static std::list<PlayableCard*> handleTargetUnitCleaveEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);

    public:
        static std::list<PlayableCard*> GetTargets(uint8_t spellTarget, Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes);
};
