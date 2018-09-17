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
typedef std::list<PlayableCard*>(*SpellTargetSelectorFunc)(Player* attacker, Player* victim, uint64_t targetGuid);

class SpellTargetSelector
{
    private:
        static SpellTargetSelectorFunc const m_spellTargetSelectors[];

        static std::list<PlayableCard*> getTargetFromDeck(Player* player, uint64_t targetGuid);

        // target handlers
        static std::list<PlayableCard*> handleTargetUnitTargetEnemy(Player* /*attacker*/, Player* victim, uint64_t targetGuid) { return getTargetFromDeck(victim, targetGuid); }
        static std::list<PlayableCard*> handleTargetUnitTargetFriend(Player* attacker, Player* /*victim*/, uint64_t targetGuid) { return getTargetFromDeck(attacker, targetGuid); }
        static std::list<PlayableCard*> handleTargetUnitCleaveEnemy(Player* /*attacker*/, Player* victim, uint64_t targetGuid);

    public:
        static std::list<PlayableCard*> GetTargets(uint8_t spellTarget, Player* attacker, Player* victim, uint64_t targetGuid);
};
