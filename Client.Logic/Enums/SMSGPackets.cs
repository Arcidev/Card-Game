
namespace Client.Logic.Enums
{
    public enum SMSGPackets
    {
        InitResponse = 0,
        GameInfo,
        AvailableCards,
        CharMessage,
        WhisperFailed,
        SelectCardsFailed,
        SelectCardsWaitForOtherPlayer,
        SelectCards,
        DeckCards,
        ActivePlayer,
        PlayerDisconnected,
        AttackResult,
        EndGame,
        CardStatChanged,
        SpellCastResult,
        SpellDamage,
        ApplyAura,
        SpellPeriodicDamage,
        CardHealed,
        ManaReplenishment,
        SpellDrain,
        SpellAuraExpired,
        SpellAuraRemoved,
        CardMorphInfo,
        UserResult,
        SwapCards
    }
}
