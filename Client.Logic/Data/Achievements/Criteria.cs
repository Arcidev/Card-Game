using System;

namespace Client.Logic.Data.Achievements
{
    public class Criteria
    {
        public UInt32 Id { get; }

        public bool IsMet { get; }

        public UInt32 Progress { get; }

        public UInt32 Requirement { get; }

        public Criteria(UInt32 id, bool isMet, UInt32 progress, UInt32 requirement)
        {
            Id = id;
            IsMet = isMet;
            Progress = progress;
            Requirement = requirement;
        }
    }
}
