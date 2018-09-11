#include <algorithm>
#include <stdexcept>
#include "DefensiveCard.h"
#include "MeleeCard.h"
#include "RangedCard.h"
#include "../DataHolder.h"
#include "../Player.h"
#include "../PlayerDefines.h"

PlayableCard::PlayableCard(uint64_t guid, Card const* card, Player* owner) : Card(*card), m_guid(guid), m_isDefending(false), m_owner(owner) { }

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
    int8_t defenseModifier = GetStatModifierValue(CARD_STAT_DEFENSE);
    if (defenseModifier < 0)
        if (GetDefense() < -defenseModifier)
            return 0;

    return GetDefense() + defenseModifier;
}

uint8_t PlayableCard::GetModifiedDamage() const
{
    int8_t damageModifier = GetStatModifierValue(CARD_STAT_DAMAGE);
    if (damageModifier < 0)
        if (GetDamage() < -damageModifier)
            return 0;

    return GetDamage() + damageModifier;
}

int8_t PlayableCard::GetStatModifierValue(uint8_t stat) const
{
    int8_t modifier = 0;
    for (auto const& aura : m_auras)
        if (aura.second.GetId() == SPELL_AURA_EFFECT_MODIFY_STAT)
            if (aura.second.GetValue1() == stat)
                modifier += (int8_t)aura.second.GetValue2();

    if ((stat == CARD_STAT_DEFENSE) && m_isDefending)
        modifier += DEFENSE_BONUS_ON_DEFEND;

    return modifier;
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

void PlayableCard::Heal(uint8_t amount)
{
    m_hp = (std::min)((uint8_t)(m_hp + amount), DataHolder::GetCard(GetId())->GetHealth());
    m_owner->SendCardHealed(this, amount);
}

std::list<uint32_t> PlayableCard::HandleTickOnAuras()
{
    std::list<uint32_t> spellIds;
    for (SpellAuraEffectsMap::iterator iter = m_auras.begin(); iter != m_auras.end();)
    {
        iter->second.Tick();
        if (!iter->second.GetDuration())
        {
            spellIds.push_back(iter->second.GetId());
            iter = m_auras.erase(iter);
        }
        else
            ++iter;
    }

    return spellIds;
}

void PlayableCard::ModifyMana(int8_t amount)
{ 
    if (amount > 0)
        m_mana = (std::min)((uint8_t)(m_mana + amount), DataHolder::GetCard(GetId())->GetMana());
    else
        m_mana = (uint8_t)(std::max)(m_mana + amount, 0);
}
