using Client.Logic.Data;
using Client.Logic.Data.Achievements;
using System;

namespace Client.UI.ViewModels.Achievements
{
    public class CriteriaViewModel
    {
        private readonly Criteria criteria;

        public UInt32 Requirement => criteria.Requirement;

        public UInt32 Progress => criteria.Progress;

        public bool IsMet => criteria.IsMet;

        public string Description { get; }

        public CriteriaViewModel(Criteria criteria, IDataHolder dataHolder)
        {
            this.criteria = criteria;
            Description = dataHolder.GetCriteriaDescription(criteria.Id);
        }
    }
}
