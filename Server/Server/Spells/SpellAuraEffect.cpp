#include "SpellAuraEffect.h"
#include "../Player.h"

SpellAuraEffect::SpellAuraEffect(PlayableCard* card, PlayableCard const* caster, uint32_t spellId, uint32_t spellValueId, uint8_t id, uint8_t duration, uint8_t value1, uint8_t value, uint32_t spellAttributes)
    : m_card(card), m_caster(caster), m_SpellAuraEffectHandler(SpellAuraEffectHandler::GetAuraEffectTickHandler(id)), m_spellId(spellId), m_spellValueId(spellValueId), m_id(id), m_duration(duration), m_currentTick(0), m_value1(value1), m_value2(value), m_spellAttributes(spellAttributes)
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
        m_SpellAuraEffectHandler(m_card, m_value2, m_spellAttributes, m_caster);
}

void SpellAuraEffect::Remove()
{
    SpellAuraEffectRemoveHandlerFunc removeHandler = SpellAuraEffectHandler::GetRemoveHandler(m_id);
    if (removeHandler)
        removeHandler((*this), m_card);
}
