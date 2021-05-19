using Arci.Networking.Data;
using System;

namespace Client.Logic.Network
{
    public static partial class PacketHandler
    {
        private static readonly Action<Packet, Game>[] packetHandlers = 
        {
            null,                       // SMSG_INIT_RESPONSE
            HandleGameInfo,             // SMSG_GAME_INFO
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
            HandleManaReplenishment,    // SMSG_MANA_REPLENISHMENT
            HandleSpellDrain,           // SMSG_SPELL_DRAIN
            HandleSpellAuraExpired,     // SMSG_SPELL_AURA_EXPIRED
            HandleSpellAuraRemoved,     // SMSG_SPELL_AURAS_REMOVED
            HandleCardMorphInfo,        // SMSG_CARD_MORPH_INFO
            HandleUserResult,           // SMSG_USER_RESULT
            HandleSwapCards             // SMSG_SWAP_CARDS
        };

        // Returns function to handle packet
        internal static Action<Packet, Game> GetPacketHandler(UInt16 packetType)
        {
            return packetType < packetHandlers.Length ? packetHandlers[packetType] : null;
        }
    }
}
