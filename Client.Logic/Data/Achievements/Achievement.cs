using System;
using System.Collections.Generic;

namespace Client.Logic.Data.Achievements
{
    public class Achievement
    {
        public UInt32 Id { get; }

        public UInt32 ParentId { get; }

        public bool IsCompleted => CompletionDate.HasValue;

        public DateTime? CompletionDate { get; }

        public IEnumerable<Criteria> Criterias { get; }

        public Achievement(UInt32 id, UInt32 parentId, DateTime? completionDate, List<Criteria> criterias)
        {
            Id = id;
            ParentId = parentId;
            CompletionDate = completionDate;
            Criterias = criterias;
        }
    }
}
