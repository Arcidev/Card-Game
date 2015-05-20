#include <string>

class ByteBuffer;
class Spell;
class StaticHelper
{
    public:
        static bool CompareStringCaseInsensitive(std::string const& str1, std::string const& str2);
        static void PackSpell(ByteBuffer* packet, Spell const* spell);
};
