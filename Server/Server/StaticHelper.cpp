#include "StaticHelper.h"
#include "PacketHandlers/ByteBuffer.h"
#include "Spells/Spell.h"

// Compares 2 strings
bool StaticHelper::CompareStringCaseInsensitive(std::string const& str1, std::string const& str2)
{
    if (str1.length() != str2.length())
        return false;

    for (unsigned int i = 0; i < str1.length(); i++)
        if (tolower(str1[i]) != tolower(str2[i]))
            return false;

    return true;
}

// Packs spell into byte buffer
void StaticHelper::PackSpell(ByteBuffer* buffer, Spell const* spell)
{
    if (!buffer || !spell)
        return;

    *buffer << spell->GetManaCost();
    *buffer << spell->GetId();
    *buffer << (uint8_t)spell->GetSpellEffects().size();
    for (std::list<SpellEffectPair>::const_iterator iter = spell->GetSpellEffects().begin(); iter != spell->GetSpellEffects().end(); ++iter)
    {

        *buffer << iter->second.Target;
        *buffer << iter->second.Value1;
        *buffer << iter->second.Value2;
        *buffer << iter->second.Value3;
        *buffer << iter->second.Value4;
    }
}
