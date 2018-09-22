#include "SpellAuraEffect.h"
#include "SpellAuraEffectHandler.h"
#include "../Player.h"

SpellAuraEffect::SpellAuraEffect(PlayableCard* card, uint32_t spellId, uint8_t id, uint8_t duration, uint8_t value1, uint8_t value, uint32_t spellAttributes)
    : m_card(card), m_SpellAuraEffectHandler(SpellAuraEffectTickHandler::GetAuraEffectTickHandler(id)), m_spellId(spellId), m_id(id), m_duration(duration), m_currentTick(0), m_value1(value1), m_value2(value), m_spellAttributes(spellAttributes)
{
    if (!m_value1)
        m_value1 = 1;
}

void SpellAuraEffect::Tick()
{
    if (!m_duration)
        return;

    --m_duration;
    if (!m_SpellAuraEffectHandler)
        return;

    m_currentTick = (m_currentTick + 1) % m_value1;
    if (!m_currentTick)
        m_SpellAuraEffectHandler(m_card, m_value2, m_spellAttributes);
}

void SpellAuraEffect::Apply(Player* player)
{
    SpellAuraEffectApplyHandlerFunc applyHandler = SpellAuraEffectHandler::GetApplyHandler(m_id);
    if (applyHandler)
        applyHandler((*this), player, m_card);
}

void SpellAuraEffect::Remove()
{
    SpellAuraEffectRemoveHandlerFunc removeHandler = SpellAuraEffectHandler::GetRemoveHandler(m_id);
    if (removeHandler)
        removeHandler((*this), m_card);
}
