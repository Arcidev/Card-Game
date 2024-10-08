﻿using System;
using System.Collections.Generic;

namespace Client.Logic.Data.Cards
{
    public class DefensiveCard : PlayableCard
    {
        public DefensiveCard(UInt64 guid, Card card, Player player) : base(guid, card, player) { }

        // Can't attack => no targets
        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex) => [];
    }
}
