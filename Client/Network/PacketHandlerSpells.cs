using Arci.Networking.Data;
using Client.Enums;
using Client.Game;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_SPELL_CAST_RESULT packet
        private static void HandleSpellCastResult(Packet packet, ClientGame game)
        {
            SpellCastResult result = (SpellCastResult)packet.ReadByte();
            if (result != SpellCastResult.Success)
            {
                game.SetActiveCardActionGrid(true);
                game.Chat.Write(result.GetDescription(), ChatTypes.Info);
                return;
            }

            var guid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(guid, 5, 7, 0, 1, 4, 3, 2, 6);

            var mana = packet.ReadByte();
            packet.ReadGuidByteStreamInOrder(guid, 7, 2);
            var manaCost = packet.ReadByte();
            packet.ReadGuidByteStreamInOrder(guid, 4, 0, 1);
            var player = game.GetPlayer(packet.ReadUInt32());
            var spellId = packet.ReadUInt32();
            packet.ReadGuidByteStreamInOrder(guid, 3, 6, 5);

            player.HandleSuccessfulSpellCast(guid, spellId, mana, manaCost);
        }

        // Handle SMSG_SPELL_DAMAGE packet
        private static void HandleSpellDamage(Packet packet, ClientGame game)
        {
            var targetsCount = packet.ReadByte();
            var targets = new PacketGuid[targetsCount];
            var isAlive = new bool[targetsCount];

            for (int i = 0; i < targetsCount; i++)
            {
                targets[i] = new PacketGuid();
                isAlive[i] = packet.ReadBit();
                packet.ReadGuidBitStreamInOrder(targets[i], 6, 3, 1, 7, 0, 2, 5, 4);
            }

            var opponent = game.GetOpponent(packet.ReadUInt32());
            for (int i = 0; i < targetsCount; i++)
            {
                packet.ReadGuidByteStreamInOrder(targets[i], 4, 3, 5);
                byte damage = packet.ReadByte();
                packet.ReadGuidByteStreamInOrder(targets[i], 2, 0, 1, 6, 7);

                if (isAlive[i])
                    opponent.AttackCard(targets[i], damage, CombatLogTypes.SpellUsage, false);
                else
                    opponent.DestroyCard(targets[i], damage, CombatLogTypes.SpellUsage, false);
            }
        }

        // Handle SMSG_APPLY_AURA packet
        private static void HandleApplyAura(Packet packet, ClientGame game)
        {
            var guid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(guid, 7, 2, 1, 3, 5, 4, 0, 6);

            var player = game.GetPlayer(packet.ReadUInt32());
            packet.ReadGuidByteStreamInOrder(guid, 0, 5, 2, 1, 7, 6, 4, 3);
            var spellId = packet.ReadUInt32();

            player.ApplyAura(guid, spellId);
        }

        // Handle SMSG_SPELL_PERIODIC_DAMAGE packet
        private static void HandleSpellPeriodicDamage(Packet packet, ClientGame game)
        {
            var guid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(guid, 6, 4, 1);
            var isAlive = packet.ReadBit();
            packet.ReadGuidBitStreamInOrder(guid, 7, 2, 3, 5, 0);

            var player = game.GetPlayer(packet.ReadUInt32());
            packet.ReadGuidByteStreamInOrder(guid, 0, 1, 2, 3, 7, 5, 4, 6);
            byte damage = packet.ReadByte();

            if (isAlive)
                player.AttackCard(guid, damage, CombatLogTypes.SpellUsage, true);
            else
                player.DestroyCard(guid, damage, CombatLogTypes.SpellUsage, true);
        }
    }
}
