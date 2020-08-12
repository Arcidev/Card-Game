#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../DataHolder.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_SELECTED_CARDS packet
void PacketHandler::handleSelectedCardsPacket(ConnectedUser* user, Packet& packet)
{
    uint32_t cardId;
    uint8_t cardCount;
    packet >> cardCount;
    if (cardCount != MAX_CARDS_COUNT)
        user->GetPlayer()->SendSelectCardsFailed(FailReson::INVALID_CARD_COUNT);

    // In case it failed in packet sended before
    user->GetPlayer()->ClearCards();
    DEBUG_LOG("CMSG_SELECTED_CARDS:\r\n\tCardCount: %d\r\n", cardCount);

    for (uint8_t i = 0; i < MAX_CARDS_COUNT; i++)
    {
        packet >> cardId;
        DEBUG_LOG("\tCardId: %d\r\n", cardId);

        Card const* card = DataHolder::GetCard(cardId);
        if (!card)
        {
            // Rest of packet is ignored
            user->GetPlayer()->SendSelectCardsFailed(FailReson::INVALID_CARD_ID);
            return;
        }

        user->GetPlayer()->CreateCard(card);
    }

    user->GetPlayer()->Prepare();
    if (!user->GetPlayer()->GetOpponent() || !user->GetPlayer()->GetOpponent()->IsPrepared())
    {
        Packet pck(SMSGPackets::SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER);
        user->SendPacket(pck);
        return;
    }

    Packet pck(SMSGPackets::SMSG_SELECT_CARDS);
    ByteBuffer userBuffer;
    ByteBuffer opponentBuffer;

    pck << (uint8_t)user->GetPlayer()->GetCards().size();
    pck << (uint8_t)user->GetPlayer()->GetOpponent()->GetCards().size();

    for (auto const& card : user->GetPlayer()->GetOpponent()->GetCards())
    {
        pck.WriteBitStreamInOrder(card.first, { 1, 2, 7, 0, 5, 3, 4, 6 });

        opponentBuffer.WriteByteStreamInOrder(card.first, { 4, 2, 6, 1, 7, 0 });
        opponentBuffer << card.second->GetId();
        opponentBuffer.WriteByteStreamInOrder(card.first, { 3, 5 });
    }

    for (auto const& card : user->GetPlayer()->GetCards())
    {
        pck.WriteBitStreamInOrder(card.first, { 7, 1, 2, 4, 6, 0, 3, 5 });

        userBuffer.WriteByteStreamInOrder(card.first, { 7, 2, 0, 1, 6, 4, 5 });
        userBuffer << card.second->GetId();
        userBuffer.WriteByteStreamInOrder(card.first, { 3 });
    }

    pck.FlushBits();
    pck << user->GetId();
    pck << userBuffer;
    pck << opponentBuffer;

    user->GetPlayer()->GetGame()->BroadcastPacket(pck);

    user->GetPlayer()->HandleDeckCards(true);
    user->GetPlayer()->GetOpponent()->HandleDeckCards(true);
    user->GetPlayer()->GetGame()->ActivateSecondPlayer();
}

// Handles CMSG_CARD_ACTION packet
void PacketHandler::handleCardActionPacket(ConnectedUser* user, Packet& packet)
{
    Guid guid;
    uint8_t attackType;
    packet.ReadBitStreamInOrder(guid, { 4, 3, 2, 7, 1, 6, 0, 5 });

    packet.ReadByteStreamInOrder(guid, { 6, 2, 7, 1, 0 });
    packet >> attackType;
    packet.ReadByteStreamInOrder(guid, { 5, 3, 4 });

    DEBUG_LOG("CMSG_CARD_ACTION:\r\n\tcardAction: %d\r\n", attackType);
    if ((CardActions)attackType == CardActions::CARD_ACTION_BASIC_ATTACK)
        user->GetPlayer()->Attack(guid);
    else if ((CardActions)attackType == CardActions::CARD_ACTION_SPELL_USE)
        user->GetPlayer()->UseSpell(guid);
}

// Handles CMSG_DEFEND_SELF packet
void PacketHandler::handleDefendSelfPacket(ConnectedUser* user, Packet& /*packet*/)
{
    DEBUG_LOG("CMSG_DEFEND_SELF\r\n");
    user->GetPlayer()->DefendSelf();
}
