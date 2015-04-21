#include "PacketHandler.h"
#include "Packet.h"
#include "Player.h"
#include "DataHolder.h"
#include "../Shared/SharedDefines.h"

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
    DEBUG_LOG("CMSG_SELECTED_CARDS:\n\tCardCount: %d\n", cardCount);
    CardsDataMap const& cards = DataHolder::GetCards();

    for (uint8_t i = 0; i < MAX_CARDS_COUNT; i++)
    {
        *packet >> cardId;
        DEBUG_LOG("\tCardId: %d\n", cardId);

        CardsDataMap::const_iterator cardIter = cards.find(cardId);
        if (cardIter == cards.end())
        {
            // Rest of packet is ignored
            player->SendSelectCardsFailed(INVALID_CARD_ID);
            return;
        }

        player->CreateCard(cardIter->second);
    }

    player->Prepare();
    if (!player->GetOpponent() || !player->GetOpponent()->IsPrepared())
    {
        Packet pck(SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER);
        player->SendPacket(&pck);
        return;
    }

    Packet pck(SMSG_SELECT_CARDS);
    pck << (uint8_t)player->GetCards().size();
    pck << (uint8_t)player->GetOpponent()->GetCards().size();

    for (CardsMap::const_iterator iter = player->GetOpponent()->GetCards().begin(); iter != player->GetOpponent()->GetCards().end(); ++iter)
        pck.WriteGuidBitStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 1, 2, 7, 0, 5, 3, 4, 6 });

    for (CardsMap::const_iterator iter = player->GetCards().begin(); iter != player->GetCards().end(); ++iter)
        pck.WriteGuidBitStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 7, 1, 2, 4, 6, 0, 3, 5 });

    pck.FlushBits();
    pck << player->GetId();

    for (CardsMap::const_iterator iter = player->GetCards().begin(); iter != player->GetCards().end(); ++iter)
    {
        pck.WriteGuidByteStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 7, 2, 0 });
        pck << iter->second.GetDamage();
        pck << iter->second.GetHealth();
        pck.WriteGuidByteStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 1, 6, 4, 5 });
        pck << iter->second.GetId();
        pck << iter->second.GetDefense();
        pck << iter->second.GetType();
        pck.WriteGuidByteStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 3 });
        pck << iter->second.GetMana();
    }

    for (CardsMap::const_iterator iter = player->GetOpponent()->GetCards().begin(); iter != player->GetOpponent()->GetCards().end(); ++iter)
    {
        pck << iter->second.GetType();
        pck << iter->second.GetDefense();
        pck << iter->second.GetDamage();
        pck.WriteGuidByteStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 4, 2, 6, 1, 7, 0 });
        pck << iter->second.GetMana();
        pck << iter->second.GetHealth();
        pck << iter->second.GetId();
        pck.WriteGuidByteStreamInOrder(iter->second.GetGuid(), std::vector<uint8_t> { 3, 5 });
    }

    player->GetGame()->BroadcastPacket(&pck);

    player->HandleDeckCards(true);
    player->GetOpponent()->HandleDeckCards(true);
    player->GetGame()->ActivateSecondPlayer();
}

void PacketHandler::handleAttackCardPacket(Player* player, Packet* packet)
{
    Guid guid;
    uint8_t attackType;
    packet->ReadGuidBitStreamInOrder(guid, std::vector<uint8_t> { 4, 3, 2, 7, 1, 6, 0, 5 });

    packet->ReadGuidByteStreamInOrder(guid, std::vector<uint8_t> { 6, 2, 7, 1, 0 });
    *packet >> attackType;
    packet->ReadGuidByteStreamInOrder(guid, std::vector<uint8_t> { 5, 3, 4 });

    /// TODO: check attack and attack card
}
