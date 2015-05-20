#pragma once
#include <cstdint>
#include <map>
#include <list>

enum SpellTargets
{
    TARGET_UNIT_CASTER = 0,
    TARGET_UNIT_TARGET_ENEMY,
    MAX_SPELL_EFFECT_TARGET
};

class PlayableCard;
class Player;
typedef std::list<PlayableCard*>(*SpellTargetSelectorFunc)(Player* attacker, Player* victim, uint64_t const& targetGuid);
typedef std::map<uint8_t, SpellTargetSelectorFunc> SpellTargetSelectorMap;

class SpellTargetSelector
{
    private:
        static SpellTargetSelectorMap m_spellTargetSelectors;

        static std::list<PlayableCard*> handleTargetUnitCaster(Player* attacker, Player* /*victim*/, uint64_t const& /*targetGuid*/);
        static std::list<PlayableCard*> handleTargetUnitTargetEnemy(Player* /*attacker*/, Player* victim, uint64_t const& targetGuid);

    public:
        static std::list<PlayableCard*> GetTargets(uint8_t const& spellTarget, Player* attacker, Player* victim, uint64_t  const& targetGuid);
};
