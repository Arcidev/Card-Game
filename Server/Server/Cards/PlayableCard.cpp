#include <stdexcept>
#include "MeleeCard.h"
#include "RangedCard.h"
#include "DefensiveCard.h"
#include "../Player.h"
#include "../PlayerDefines.h"

PlayableCard::PlayableCard(uint64_t const& guid, Card const* card, Player* owner) : Card(*card), m_guid(guid), m_isDefending(false), m_owner(owner) { }

PlayableCard* PlayableCard::Create(uint64_t const& guid, Card const* card, Player* owner)
{
    switch (card->GetType())
    {
        case CARD_TYPE_MELEE:
            return new MeleeCard(guid, card, owner);
        case CARD_TYPE_RANGED:
            return new RangedCard(guid, card, owner);
        case CARD_TYPE_DEFENSE:
            return new DefensiveCard(guid, card, owner);
        default:
            throw std::invalid_argument("Card type not exists");
    }
}

void PlayableCard::SetDefendState(bool const& defend)
{
    if (m_isDefending == defend)
        return;

    m_isDefending = defend;
    m_owner->SendCardStatChanged(this, CARD_STAT_DEFENSE);  
}

uint8_t PlayableCard::GetModifiedDefense() const
{
    int8_t defenseModifier = GetDefenseModifier();
    if (defenseModifier < 0)
        if (m_defense < -defenseModifier)
            return 0;

    return m_defense + defenseModifier;
}

int8_t PlayableCard::GetDefenseModifier() const
{
    int8_t modifier = 0;
    for (SpellAuraEffectsMap::const_iterator iter = m_auras.begin(); iter != m_auras.end(); ++iter)
        if (iter->second.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
            if (iter->second.GetValue1() == CARD_STAT_DEFENSE)
                modifier += (int8_t)iter->second.GetValue2();

    if (m_isDefending)
        modifier += DEFENSE_BONUS_ON_DEFEND;

    return modifier;
}

uint8_t PlayableCard::GetModifiedDamage() const
{
    int8_t damageModifier = GetDamageModifier();
    if (damageModifier < 0)
        if (m_damage < -damageModifier)
            return 0;

    return m_damage + damageModifier;
}

int8_t PlayableCard::GetDamageModifier() const
{
    int8_t modifier = 0;
    for (SpellAuraEffectsMap::const_iterator iter = m_auras.begin(); iter != m_auras.end(); ++iter)
        if (iter->second.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
            if (iter->second.GetValue1() == CARD_STAT_DAMAGE)
                modifier += (int8_t)iter->second.GetValue2();

    return modifier;
}

int8_t PlayableCard::GetStatModifierValue(uint8_t const& stat) const
{
    switch (stat)
    {
        case CARD_STAT_DEFENSE:
            return GetDefenseModifier();
        case CARD_STAT_DAMAGE:
            return GetDamageModifier();
        default:
            return 0;
    }
}

void PlayableCard::ApplyAura(SpellAuraEffect const& aura)
{
    SpellAuraEffectsMap::iterator iter = m_auras.find(aura.GetSpellId());
    if (iter != m_auras.end())
        iter->second = aura;
    else
        m_auras.insert(std::make_pair(aura.GetSpellId(), aura));

    if (aura.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
        m_owner->SendCardStatChanged(this, aura.GetValue1());

    m_owner->SendApplyAura(m_guid, &aura);
}
