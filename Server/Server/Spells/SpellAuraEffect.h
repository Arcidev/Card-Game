#pragma once
#include "SpellAuraEffectHandler.h"

class Player;
class PlayableCard;

class SpellAuraEffect
{
    private:
        PlayableCard* m_card;
        PlayableCard const* m_caster;
        SpellAuraEffectTickHandlerFunc m_SpellAuraEffectHandler;
        uint32_t m_spellId;
        uint32_t m_spellValueId;
        uint8_t m_id;
        uint8_t m_duration;
        uint8_t m_currentTick;
        uint8_t m_value1;
        uint8_t m_value2;
        uint32_t m_spellAttributes;

    public:
        SpellAuraEffect(PlayableCard* card, PlayableCard const* caster, uint32_t spellId, uint32_t spellValueId, uint8_t id, uint8_t duration, uint8_t value1, uint8_t value2, uint32_t spellAttributes);
        void Tick();
        void Remove();

        uint32_t GetSpellId() const { return m_spellId; }
        uint32_t GetSpellValueId() const { return m_spellValueId; }
        uint8_t GetId() const { return m_id; }
        uint8_t GetValue1() const { return m_value1; }
        uint8_t GetValue2() const { return m_value2; }
        uint8_t GetDuration() const { return m_duration; }
        uint32_t GetSpellAttributes() const { return m_spellAttributes; }
};
