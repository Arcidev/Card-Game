using Arci.Networking.Data;
using Client.Game;
using System;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        private static readonly Action<Packet, ClientGame>[] packetHandlers = 
        {
            HandleInitResponse,         // SMSG_INIT_RESPONSE
            HandleAvailableCards,       // SMSG_AVAILABLE_CARDS
            HandleChatMessage,          // SMSG_CHAT_MESSAGE
            HandleWhisperFailed,        // SMSG_WHISPER_FAILED
            HandleSelectCardsFailed,    // SMSG_SELECT_CARDS_FAILED
            HandleSelectCardsWait,      // SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER
            HandleSelectCards,          // SMSG_SELECT_CARDS
            HandleDeckCards,            // SMSG_DECK_CARDS
            HandleActivePlayer,         // SMSG_ACTIVE_PLAYER
            HandlePlayerDisconnected,   // SMSG_PLAYER_DISCONNECTED
            HandleAttackResult,         // SMSG_ATTACK_RESULT
            HandleEndGame,              // SMSG_END_GAME
            HandleCardStatChanged,      // SMSG_CARD_STAT_CHANGED
            HandleSpellCastResult,      // SMSG_SPELL_CAST_RESULT
            HandleSpellDamage,          // SMSG_SPELL_DAMAGE
            HandleApplyAura,            // SMSG_APPLY_AURA
            HandleSpellPeriodicDamage,  // SMSG_SPELL_PERIODIC_DAMAGE
            HandleCardHealed,           // SMSG_CARD_HEALED
            HandleManaReplenishment     // SMSG_MANA_REPLENISHMENT
        };

        // Returns function to handle packet
        public static Action<Packet, ClientGame> GetPacketHandler(UInt16 packetType)
        {
            return packetHandlers[packetType];
        }
    }
}
