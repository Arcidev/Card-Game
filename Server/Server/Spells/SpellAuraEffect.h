#pragma once
#include "SpellAuraEffectHandler.h"

class PlayableCard;
class SpellAuraEffect
{
    private:
        PlayableCard* m_card;
        SpellAuraEffectHandlerFunc m_SpellAuraEffectHandler;
        uint32_t m_spellId;
        uint8_t m_id;
        uint8_t m_duration;
        uint8_t m_currentTick;
        uint8_t m_value1;
        uint8_t m_value2;

    public:
        SpellAuraEffect(PlayableCard* card, uint32_t const& m_spellId, uint8_t const& id, uint8_t const& duration, uint8_t const& value1, uint8_t const& value2);
        void Tick();

        uint32_t const& GetSpellId() const { return m_spellId; }
        uint8_t const& GetId() const { return m_id; }
        uint8_t const& GetValue1() const { return m_value1; }
        uint8_t const& GetValue2() const { return m_value2; }
};
