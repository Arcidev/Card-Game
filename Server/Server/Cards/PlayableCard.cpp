#include <algorithm>
#include <stdexcept>
#include "DefensiveCard.h"
#include "MeleeCard.h"
#include "RangedCard.h"
#include "../DataHolder.h"
#include "../Player.h"
#include "../PlayerDefines.h"
#include "../Spells/SpellAuraEffectHandler.h"
#include "../Spells/SpellDefines.h"

PlayableCard::PlayableCard(uint64_t guid, Card const* card, Player* owner) : Card(*card), m_guid(guid), m_isDefending(false), m_owner(owner), m_morph(nullptr) { }

PlayableCard* PlayableCard::Create(uint64_t guid, Card const* card, Player* owner)
{
    switch (card->GetType())
    {
        case CardType::CARD_TYPE_MELEE:
            return new MeleeCard(guid, card, owner);
        case CardType::CARD_TYPE_RANGED:
            return new RangedCard(guid, card, owner);
        case CardType::CARD_TYPE_DEFENSE:
            return new DefensiveCard(guid, card, owner);
        default:
            throw std::invalid_argument("Card type not exists");
    }
}

void PlayableCard::SetDefendState(bool defend)
{
    if (m_isDefending == defend)
        return;

    m_isDefending = defend;
    m_owner->SendCardStatChanged(this, CardStats::CARD_STAT_DEFENSE);
}

uint8_t PlayableCard::GetModifiedDefense() const
{
    uint8_t baseDefense = m_morph ? m_morph->GetDefense() : GetDefense();
    int8_t defenseModifier = GetStatModifierValue(CardStats::CARD_STAT_DEFENSE);
    return (std::max)(baseDefense + defenseModifier, 0);
}

uint8_t PlayableCard::GetModifiedDamage(uint8_t additional_modifier) const
{
    uint8_t baseDamage = m_morph ? m_morph->GetDamage() : GetDamage();
    int8_t damageModifier = GetStatModifierValue(CardStats::CARD_STAT_DAMAGE);

    return (std::max)(baseDamage + damageModifier + additional_modifier, 0);
}

int8_t PlayableCard::GetStatModifierValue(CardStats stat) const
{
    int8_t modifier = 0;
    for (auto const& aura : m_auras)
        if (aura.second.GetDuration() && aura.second.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
            if (aura.second.GetValue1() & (uint8_t)stat)
                modifier += (int8_t)aura.second.GetValue2();

    if ((stat & CardStats::CARD_STAT_DEFENSE) && m_isDefending)
        modifier += (uint8_t)SystemStats::DEFENSE_BONUS_ON_DEFEND;

    return modifier;
}

SpellAuraEffect const& PlayableCard::ApplyAura(SpellAuraEffect const& aura)
{
    if (aura.GetSpellAttributes() & SPELL_ATTRIBUTE_AURA_EXCLUSIVE)
        RemoveAurasByType(aura.GetId(), SIZE_MAX);
    else
        removeExclusiveAura(aura.GetId());

    m_owner->SendApplyAura(m_guid, aura);

    auto auraKey = std::make_pair(aura.GetSpellId(), aura.GetSpellValueId());
    SpellAuraEffectsMap::iterator iter = m_auras.find(auraKey);
    if (iter != m_auras.end())
    {
        iter->second = aura;
        return iter->second;
    }
        
    return m_auras.insert(std::make_pair(auraKey, aura)).first->second;
}

void PlayableCard::RemoveAurasByType(uint8_t auraTypeId, size_t toRemove)
{
    std::list<uint32_t> removedSpellIds;
    for (SpellAuraEffectsMap::iterator iter = m_auras.begin(); iter != m_auras.end();)
    {
        if (iter->second.GetId() != auraTypeId || iter->second.GetSpellAttributes() & SPELL_ATTRIBUTE_AURA_UNREMOVABLE)
        {
            ++iter;
            continue;
        }

        removedSpellIds.push_back(iter->first.first);
        iter->second.Remove();
        iter = m_auras.erase(iter);

        if (!(--toRemove))
            break;
    }

    m_owner->SendAurasRemoved(m_guid, removedSpellIds);
}

void PlayableCard::removeExclusiveAura(uint8_t auraTypeId)
{
    for (SpellAuraEffectsMap::iterator iter = m_auras.begin(); iter != m_auras.end(); iter++)
    {
        if (iter->second.GetId() != auraTypeId)
            continue;

        // Only 1 exclusive aura should be active per type (without any non-exclusive), therefore we check only first match
        if (~iter->second.GetSpellAttributes() & SPELL_ATTRIBUTE_AURA_EXCLUSIVE)
            return;

        m_owner->SendAurasRemoved(m_guid, { iter->first.first });
        m_auras.erase(iter);
        return;
    }
}

void PlayableCard::Heal(uint8_t amount)
{
    AddHealth(amount);
    m_owner->SendCardHealed(this, amount);
}

void PlayableCard::HandleTickOnAuras()
{
    for (SpellAuraEffectsMap::iterator iter = m_auras.begin(); iter != m_auras.end();)
    {
        iter->second.Tick();
        if (iter->second.GetDuration())
        {
            ++iter;
            continue;
        }

        iter->second.Remove(); 
        m_owner->SendAuraExpired(m_guid, iter->second);
        iter = m_auras.erase(iter);
    }
}

void PlayableCard::AddMana(uint8_t amount)
{ 
    m_mana = (std::min)(m_mana + amount, (int)DataHolder::GetCard(GetId())->GetMana());
}

void PlayableCard::AddHealth(uint8_t amount)
{
    m_hp = (std::min)(m_hp + amount, (int)DataHolder::GetCard(GetId())->GetHealth());
}
