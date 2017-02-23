#include <cinttypes>
#include "PacketHandler.h"
#include "Packet.h"
#include "../Player.h"
#include "../DataHolder.h"
#include "../Cards/PlayableCard.h"
#include "../StaticHelper.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_SELECTED_CARDS packet
void PacketHandler::handleSelectedCardsPacket(Player* player, Packet* packet)
{
    uint32_t cardId;
    uint8_t cardCount;
    *packet >> cardCount;
    if (cardCount != MAX_CARDS_COUNT)
        player->SendSelectCardsFailed(INVALID_CARD_COUNT);

    // In case it failed in packet sended before
    player->ClearCards();
    DEBUG_LOG("CMSG_SELECTED_CARDS:\r\n\tCardCount: %d\r\n", cardCount);

    for (uint8_t i = 0; i < MAX_CARDS_COUNT; i++)
    {
        *packet >> cardId;
        DEBUG_LOG("\tCardId: %d\r\n", cardId);

        Card const* card = DataHolder::GetCard(cardId);
        if (!card)
        {
            // Rest of packet is ignored
            player->SendSelectCardsFailed(INVALID_CARD_ID);
            return;
        }

        player->CreateCard(card);
    }

    player->Prepare();
    if (!player->GetOpponent() || !player->GetOpponent()->IsPrepared())
    {
        Packet pck(SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER);
        player->SendPacket(&pck);
        return;
    }

    Packet pck(SMSG_SELECT_CARDS);
    ByteBuffer playerBuffer;
    ByteBuffer opponentBuffer;

    pck << (uint8_t)player->GetCards().size();
    pck << (uint8_t)player->GetOpponent()->GetCards().size();

    for (CardsMap::const_iterator iter = player->GetOpponent()->GetCards().begin(); iter != player->GetOpponent()->GetCards().end(); ++iter)
    {
        pck.WriteBitStreamInOrder(iter->first, { 1, 2, 7, 0, 5, 3, 4, 6 });

        opponentBuffer.WriteByteStreamInOrder(iter->first, { 4, 2, 6, 1, 7, 0 });
        opponentBuffer << iter->second->GetId();
        opponentBuffer.WriteByteStreamInOrder(iter->first, { 3, 5 });
    }

    for (CardsMap::const_iterator iter = player->GetCards().begin(); iter != player->GetCards().end(); ++iter)
    {
        pck.WriteBitStreamInOrder(iter->first, { 7, 1, 2, 4, 6, 0, 3, 5 });

        playerBuffer.WriteByteStreamInOrder(iter->first, { 7, 2, 0, 1, 6, 4, 5 });
        playerBuffer << iter->second->GetId();
        playerBuffer.WriteByteStreamInOrder(iter->first, { 3 });
    }

    pck.FlushBits();
    pck << player->GetId();
    pck << playerBuffer;
    pck << opponentBuffer;

    player->GetGame()->BroadcastPacket(&pck);

    player->HandleDeckCards(true);
    player->GetOpponent()->HandleDeckCards(true);
    player->GetGame()->ActivateSecondPlayer();
}

// Handles CMSG_CARD_ACTION packet
void PacketHandler::handleCardActionPacket(Player* player, Packet* packet)
{
    Guid guid;
    uint8_t attackType;
    packet->ReadBitStreamInOrder(guid, { 4, 3, 2, 7, 1, 6, 0, 5 });

    packet->ReadByteStreamInOrder(guid, { 6, 2, 7, 1, 0 });
    *packet >> attackType;
    packet->ReadByteStreamInOrder(guid, { 5, 3, 4 });

    DEBUG_LOG("CMSG_CARD_ACTION:\r\n\tcardAction: %d\r\n", attackType);
    if (attackType == CARD_ACTION_BASIC_ATTACK)
        player->Attack(guid);
    else if (attackType == CARD_ACTION_SPELL_USE)
        player->UseSpell(guid);
}

// Handles CMSG_DEFEND_SELF packet
void PacketHandler::handleDefendSelfPacket(Player* player, Packet* /*packet*/)
{
    DEBUG_LOG("CMSG_DEFEND_SELF\r\n");
    player->DefendSelf();
}
