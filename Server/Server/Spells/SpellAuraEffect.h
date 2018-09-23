#pragma once
#include "SpellAuraEffectTickHandler.h"

class Player;
class PlayableCard;
class SpellAuraEffect
{
    private:
        PlayableCard* m_card;
        SpellAuraEffectTickHandlerFunc m_SpellAuraEffectHandler;
        uint32_t m_spellId;
        uint8_t m_id;
        uint8_t m_duration;
        uint8_t m_currentTick;
        uint8_t m_value1;
        uint8_t m_value2;
        uint32_t m_spellAttributes;

    public:
        SpellAuraEffect(PlayableCard* card, uint32_t m_spellId, uint8_t id, uint8_t duration, uint8_t value1, uint8_t value2, uint32_t spellAttributes);
        void Tick();
        void Apply(Player* caster);
        void Remove();

        uint32_t GetSpellId() const { return m_spellId; }
        uint8_t GetId() const { return m_id; }
        uint8_t GetValue1() const { return m_value1; }
        uint8_t GetValue2() const { return m_value2; }
        uint8_t GetDuration() const { return m_duration; }
};
