#include "SpellAuraEffect.h"

SpellAuraEffect::SpellAuraEffect(PlayableCard* card, uint32_t const& spellId, uint8_t const& id, uint8_t const& duration, uint8_t const& value1, uint8_t const& value)
    : m_card(card), m_SpellAuraEffectHandler(SpellAuraEffectHandler::GetAuraEffectTickHandler(id)), m_spellId(spellId), m_id(id), m_duration(duration), m_currentTick(0), m_value1(value1), m_value2(value)
{
    if (!m_SpellAuraEffectHandler)
        m_duration = 0;
}

void SpellAuraEffect::Tick()
{
    if (!m_SpellAuraEffectHandler)
        return;

    if (++m_currentTick >= m_value1)
        m_SpellAuraEffectHandler(m_card, m_value2);
}
