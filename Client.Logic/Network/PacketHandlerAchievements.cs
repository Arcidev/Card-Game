using Arci.Networking.Data;
using Client.Logic.Data.Achievements;
using System;
using System.Collections.Generic;

namespace Client.Logic.Network
{
    public static partial class PacketHandler
    {
        /// <summary>
        /// Handles SMSG_ACHIEVEMENTS
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleAchievements(Packet packet, Game game)
        {
            var achievementsCount = packet.ReadUInt16();
            var achievements = new List<Achievement>(achievementsCount);
            for (var i = 0; i < achievementsCount; i++)
            {
                var id = packet.ReadUInt32();
                var parentId = packet.ReadUInt32();
                var isCompleted = packet.ReadByte() != 0;

                var criterias = new List<Criteria>();
                achievements.Add(new Achievement(id, parentId, isCompleted, criterias));

                var criteriaCount = packet.ReadByte();
                for (var j = 0; j < criteriaCount; j++)
                {
                    var criteriaId = packet.ReadUInt32();
                    var isMet = packet.ReadByte() != 0;

                    UInt32 progress = 0;
                    UInt32 requirement = 0;
                    if (!isMet)
                    {
                        progress = packet.ReadUInt32();
                        requirement = packet.ReadUInt32();
                    }

                    criterias.Add(new Criteria(criteriaId, isMet, progress, requirement));
                }
            }

            game.SetAchievements(achievements);
        }

        /// <summary>
        /// Handles SMSG_ACHIEVEMENT_EARNED
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleAchievementEarned(Packet packet, Game game)
        {
            var id = packet.ReadUInt32();
            var achievementInfo = game.DataHolder.GetAchievementInfo(id);
            game.OnAchievementEarned(achievementInfo.Name);
        }
    }
}
