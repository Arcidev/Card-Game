
namespace Client.Logic.Enums
{
    public enum SMSGPackets
    {
        InitResponse = 0,
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
        UserResult
    }
}
