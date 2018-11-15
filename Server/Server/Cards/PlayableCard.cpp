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

void PlayableCard::SetDefendState(bool defend)
{
    if (m_isDefending == defend)
        return;

    m_isDefending = defend;
    m_owner->SendCardStatChanged(this, CARD_STAT_DEFENSE);  
}

uint8_t PlayableCard::GetModifiedDefense() const
{
    uint8_t baseDefense = m_morph ? m_morph->GetDefense() : GetDefense();
    int8_t defenseModifier = GetStatModifierValue(CARD_STAT_DEFENSE);
    if (defenseModifier < 0)
        if (baseDefense < -defenseModifier)
            return 0;

    return baseDefense + defenseModifier;
}

uint8_t PlayableCard::GetModifiedDamage() const
{
    uint8_t baseDamage = m_morph ? m_morph->GetDamage() : GetDamage();
    int8_t damageModifier = GetStatModifierValue(CARD_STAT_DAMAGE);
    if (damageModifier < 0)
        if (baseDamage < -damageModifier)
            return 0;

    return baseDamage + damageModifier;
}

int8_t PlayableCard::GetStatModifierValue(uint8_t stat) const
{
    int8_t modifier = 0;
    for (auto const& aura : m_auras)
        if (aura.second.GetDuration() && aura.second.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
            if (aura.second.GetValue1() == stat)
                modifier += (int8_t)aura.second.GetValue2();

    if ((stat == CARD_STAT_DEFENSE) && m_isDefending)
        modifier += DEFENSE_BONUS_ON_DEFEND;

    return modifier;
}

SpellAuraEffect const& PlayableCard::ApplyAura(SpellAuraEffect const& aura)
{
    m_owner->SendApplyAura(m_guid, aura);

    SpellAuraEffectsMap::iterator iter = m_auras.find(aura.GetSpellId());
    if (iter != m_auras.end())
    {
        iter->second = aura;
        return iter->second;
    }
        
    return m_auras.insert(std::make_pair(aura.GetSpellId(), aura)).first->second;
}

std::list<uint32_t> PlayableCard::RemoveAurasByType(uint8_t auraTypeId, bool removeFirstOnly)
{
    std::list<uint32_t> removedSpellIds;
    for (SpellAuraEffectsMap::iterator iter = m_auras.begin(); iter != m_auras.end();)
    {
        if (iter->second.GetId() != auraTypeId)
        {
            ++iter;
            continue;
        }

        removedSpellIds.push_back(iter->first);
        iter->second.Remove();
        iter = m_auras.erase(iter);

        if (removeFirstOnly)
            break;
    }

    return removedSpellIds;
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
