using System;
using System.Collections.Generic;

namespace Client.Logic.Data.Achievements
{
    public record Achievement(UInt32 Id, UInt32 ParentId, DateTime? CompletionDate, IReadOnlyCollection<Criteria> Criterias)
    {
        public bool IsCompleted => CompletionDate.HasValue;
    }
}
