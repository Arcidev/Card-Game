#include <iostream>
#include "DataHolder.h"
#include "Spells/SpellDefines.h"
#include "Spells/SpellValidator.h"

CardsDataMap DataHolder::m_cards;
DataHolder::SpellsDataMap DataHolder::m_spells;

// Callback for selecting spells
void DataHolder::loadSpellsCallback(PGresult const* result)
{
    for (int i = 0; i < PQntuples(result); i++)
    {
        uint32_t spellId = (uint32_t)strtoul(PQgetvalue(result, i, 0), nullptr, 0);
        uint8_t targetId = (uint8_t)strtoul(PQgetvalue(result, i, 4), nullptr, 0);
        if (targetId >= MAX_SPELL_EFFECT_TARGET)
        {
            std::cerr << "Spell id " << spellId << " has invalid target" << std::endl;
            continue;
        }

        uint8_t spellEffectId = (uint8_t)strtoul(PQgetvalue(result, i, 2), nullptr, 0);
        SpellEffectFunc spellEffectFunc = SpellEffect::GetSpellEffectFunc(spellEffectId);
        if (!spellEffectFunc)
        {
            std::cerr << "Spell id " << spellId << " has invalid spell effect" << std::endl;
            continue;
        }

        SpellsDataMap::iterator spellIter = m_spells.find(spellId);
        if (spellIter == m_spells.end())
            spellIter = m_spells.insert(std::make_pair(spellId, Spell(spellId, (uint32_t)strtoul(PQgetvalue(result, i, 5), nullptr, 0)))).first;

        uint32_t spellValueId = (uint32_t)strtoul(PQgetvalue(result, i, 1), nullptr, 0);
        uint32_t spellAttributes = (uint32_t)strtoul(PQgetvalue(result, i, 3), nullptr, 0);
        uint32_t value1 = (uint32_t)strtoul(PQgetvalue(result, i, 6), nullptr, 0);
        uint32_t value2 = (uint32_t)strtoul(PQgetvalue(result, i, 7), nullptr, 0);
        uint32_t value3 = (uint32_t)strtoul(PQgetvalue(result, i, 8), nullptr, 0);
        uint32_t value4 = (uint32_t)strtoul(PQgetvalue(result, i, 9), nullptr, 0);

        SpellEffectValues spellVal(spellId, spellValueId, spellAttributes, targetId, value1, value2, value3, value4);
        spellIter->second.addSpellEffect(std::make_pair(spellEffectFunc, spellVal));

        SpellValidator::ValidateSpellEffect(spellEffectId, spellVal);
    }
    
}

// Callback for selecting cards
void DataHolder::loadCardsCallback(PGresult const* result)
{
    for (int i = 0; i < PQntuples(result); i++)
    {
        Spell const* spell = nullptr;
        if (PQgetvalue(result, i, 7))
        {
            SpellsDataMap::const_iterator spellIter = m_spells.find((uint32_t)strtoul(PQgetvalue(result, i, 7), nullptr, 0));
            if (spellIter != m_spells.end())
                spell = &spellIter->second;
        }

        uint32_t id = (uint32_t)strtoul(PQgetvalue(result, i, 0), nullptr, 0);
        CardType type = (CardType)strtoul(PQgetvalue(result, i, 1), nullptr, 0);
        uint8_t hp = (uint8_t)strtoul(PQgetvalue(result, i, 2), nullptr, 0);
        uint8_t damage = (uint8_t)strtoul(PQgetvalue(result, i, 3), nullptr, 0);
        uint8_t mana = (uint8_t)strtoul(PQgetvalue(result, i, 4), nullptr, 0);
        uint8_t defense = (uint8_t)strtoul(PQgetvalue(result, i, 5), nullptr, 0);
        uint8_t price = (uint8_t)strtoul(PQgetvalue(result, i, 6), nullptr, 0);

        Card card(id, type, hp, damage, mana, defense, price, spell);
        m_cards.insert(std::make_pair(card.GetId(), card));
    }
}

// Loads spells from database
void DataHolder::loadSpells()
{
    PreparedStatement stmt("SELECT spells.Id, spell_values.Id, spell_effect_id, spell_attributes_mask, target, mana_cost, effect_value_1, effect_value_2, effect_value_3, effect_value_4 FROM (spells JOIN spells_spell_values ON spells.id = spell_id) JOIN spell_values ON spell_value_id = spell_values.id");
    DatabaseInstance::GetDbCommandHandler().ExecuteCommand(stmt, loadSpellsCallback);
}

// Loads cards info from database
void DataHolder::loadCards()
{
    PreparedStatement stmt("SELECT id, type, base_hp, base_damage, base_mana, base_defense, price, spell_id FROM cards");
    DatabaseInstance::GetDbCommandHandler().ExecuteCommand(stmt, loadCardsCallback);
}

// Loads all data from database
void DataHolder::LoadData()
{
    loadSpells();
    loadCards();
}

Card const* DataHolder::GetCard(uint32_t cardId)
{
    CardsDataMap::const_iterator iter = m_cards.find(cardId);
    return iter != m_cards.end() ? &iter->second : nullptr;
}
