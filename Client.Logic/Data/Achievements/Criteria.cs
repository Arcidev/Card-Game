using System;

namespace Client.Logic.Data.Achievements
{
    public record Criteria(UInt32 Id, bool IsMet, UInt32 Progress, UInt32 Requirement)
    {

    }
}
