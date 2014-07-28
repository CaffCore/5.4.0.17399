/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include "AuctionHouseMgr.h"
#include "Log.h"
#include "Language.h"
#include "Opcodes.h"
#include "UpdateMask.h"
#include "Util.h"
#include "AccountMgr.h"

//please DO NOT use iterator++, because it is slower than ++iterator!!!
//post-incrementation is always slower than pre-incrementation !

//void called when player click on auctioneer npc
void WorldSession::HandleAuctionHelloOpcode(WorldPacket& recvData)
{
	Player* p = GetPlayer();				//  Just a hack, but this way WorldSession::HandleAuctionHelloOpcode
    uint64 sel_guid = p->GetSelection();	//  should never have to be updated for future patches.

    //uint64 guid;                                            //NPC guid
    //recvData >> guid;	

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(sel_guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!unit)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionHelloOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(sel_guid)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendAuctionHello(sel_guid, unit);
}

//this void causes that auction window is opened
void WorldSession::SendAuctionHello(uint64 guid, Creature* unit)
{
    if (GetPlayer()->getLevel() < sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ))
    {
        SendNotification(GetTrinityString(LANG_AUCTION_REQ), sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ));
        return;
    }

    AuctionHouseEntry const* ahEntry = AuctionHouseMgr::GetAuctionHouseEntry(unit->getFaction());
    if (!ahEntry)
        return;

    WorldPacket data(SMSG_AUCTION_HELLO_REPONSE, 12);
    ObjectGuid l_Guid = guid;

    data.WriteBit(l_Guid[6]);
    data.WriteBit(l_Guid[1]);
    data.WriteBit(l_Guid[4]);
    data.WriteBit(l_Guid[0]);
    data.WriteBit(l_Guid[2]);
    data.WriteBit(1);                                      // 3.3.3: 1 - AH enabled, 0 - AH disabled
    data.WriteBit(l_Guid[3]);
    data.WriteBit(l_Guid[5]);
    data.WriteBit(l_Guid[7]);
    data.FlushBits();
    data.WriteByteSeq(l_Guid[2]);
    data.WriteByteSeq(l_Guid[0]);
    data.WriteByteSeq(l_Guid[5]);
    data.WriteByteSeq(l_Guid[3]);
    data.WriteByteSeq(l_Guid[6]);
    data.WriteByteSeq(l_Guid[1]);
    data.WriteByteSeq(l_Guid[7]);

    data << uint32(ahEntry->houseId);

    data.WriteByteSeq(l_Guid[4]);
    SendPacket(&data);
}

//call this method when player bids, creates, or deletes auction
void WorldSession::SendAuctionCommandResult(AuctionEntry* auction, uint32 action, uint32 errorCode, uint32 bidError)
{
    WorldPacket data(SMSG_AUCTION_COMMAND_RESULT);
    ObjectGuid guid = uint64(auction->bid ? auction->GetAuctionOutBid() : 0);

    data << uint32(action);
    data << uint32(bidError);
    data << uint32(auction ? auction->Id : 0);
    data << uint32(errorCode);
    
    data.WriteBit(errorCode != ERR_AUCTION_HIGHER_BID);
    data.WriteBit(uint64(guid) == 0);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(errorCode != ERR_AUCTION_HIGHER_BID);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    if(errorCode == ERR_AUCTION_HIGHER_BID)
    {
        data << uint64(auction->bidder);
        data << uint64(auction->bid);
    }

    SendPacket(&data);
}

//this function sends notification, if bidder is online
void WorldSession::SendAuctionBidderNotification(uint32 location, uint32 auctionId, uint64 bidder, uint32 bidSum, uint32 diff, uint32 itemEntry)
{
    WorldPacket data(SMSG_AUCTION_BIDDER_NOTIFICATION, (8*4));
    data << uint32(location);
    data << uint32(auctionId);
    data << uint64(bidder);
    data << uint64(bidSum);
    data << uint64(diff);
    data << uint32(itemEntry);
    data << uint32(0);
    SendPacket(&data);
}

// this void causes on client to display: "Your auction sold"
void WorldSession::SendAuctionOwnerNotification(AuctionEntry* auction)
{
    WorldPacket data(SMSG_AUCTION_OWNER_NOTIFICATION, 40);
    data << uint32(auction->Id);
    data << uint64(auction->bid);
    data << uint64(0);                                     //unk
    data << uint64(0);                                     //unk
    data << uint32(auction->itemEntry);
    data << uint32(0);                                     //unk
    data << float(0);                                      //unk
    SendPacket(&data);
}

void WorldSession::SendAuctionRemovedNotification(uint32 auctionId, uint32 itemEntry, int32 randomPropertyId)
{
    WorldPacket data(SMSG_AUCTION_REMOVED_NOTIFICATION, (4+4+4));
    data << uint32(auctionId);
    data << uint32(itemEntry);
    data << uint32(randomPropertyId);
    SendPacket(&data);
}

//this void creates new auction and adds auction to some auctionhouse
void WorldSession::HandleAuctionSellItem(WorldPacket& recvData)
{
    uint64 bid, buyout;
    uint32 itemsCount, etime;
    ObjectGuid auctioneer;
    recvData >> bid >> buyout;
    recvData >> etime;
    auctioneer[3] = recvData.ReadBit();
    auctioneer[0] = recvData.ReadBit();
    auctioneer[7] = recvData.ReadBit();
    auctioneer[2] = recvData.ReadBit();
    auctioneer[6] = recvData.ReadBit();
    auctioneer[5] = recvData.ReadBit();

    itemsCount = recvData.ReadBits(5);
    if (itemsCount > MAX_AUCTION_ITEMS)
    {
        SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
        recvData.rfinish();
        return;
    }

    ObjectGuid itemGUIDs[MAX_AUCTION_ITEMS]; // 160 slot = 4x 36 slot bag + backpack 16 slot
    uint32 count[MAX_AUCTION_ITEMS];

    for(uint32 i = 0 ; i < itemsCount ; i++)
    {
        itemGUIDs[i][0] = recvData.ReadBit();
        itemGUIDs[i][2] = recvData.ReadBit();
        itemGUIDs[i][7] = recvData.ReadBit();
        itemGUIDs[i][4] = recvData.ReadBit();
        itemGUIDs[i][6] = recvData.ReadBit();
        itemGUIDs[i][3] = recvData.ReadBit();
        itemGUIDs[i][5] = recvData.ReadBit();
        itemGUIDs[i][1] = recvData.ReadBit();
    }

    auctioneer[4] = recvData.ReadBit();
    auctioneer[1] = recvData.ReadBit();

    for(uint32 i = 0 ; i < itemsCount ; i++)
    {
        recvData.ReadByteSeq(itemGUIDs[i][6]);
        recvData.ReadByteSeq(itemGUIDs[i][7]);
        recvData.ReadByteSeq(itemGUIDs[i][4]);
        recvData.ReadByteSeq(itemGUIDs[i][1]);
        recvData.ReadByteSeq(itemGUIDs[i][0]);
        recvData.ReadByteSeq(itemGUIDs[i][2]);
        recvData >> count[i];
        recvData.ReadByteSeq(itemGUIDs[i][3]);
        recvData.ReadByteSeq(itemGUIDs[i][5]);

        if (!itemGUIDs[i] || !count[i] || count[i] > 1000)
        {
            recvData.rfinish();
            return;
        }
    }

    recvData.ReadByteSeq(auctioneer[7]);
    recvData.ReadByteSeq(auctioneer[1]);
    recvData.ReadByteSeq(auctioneer[2]);
    recvData.ReadByteSeq(auctioneer[5]);
    recvData.ReadByteSeq(auctioneer[6]);
    recvData.ReadByteSeq(auctioneer[4]);
    recvData.ReadByteSeq(auctioneer[0]);
    recvData.ReadByteSeq(auctioneer[3]);

    if (!bid || !etime)
        return;

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) not found or you can't interact with him.", GUID_LOPART(auctioneer));
        return;
    }

    AuctionHouseEntry const* auctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntry(creature->getFaction());
    if (!auctionHouseEntry)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) has wrong faction.", GUID_LOPART(auctioneer));
        return;
    }

    etime *= MINUTE;

    switch (etime)
    {
        case 1*MIN_AUCTION_TIME:
        case 2*MIN_AUCTION_TIME:
        case 4*MIN_AUCTION_TIME:
            break;
        default:
            return;
    }

    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Item* items[MAX_AUCTION_ITEMS];

    uint32 finalCount = 0;

    for (uint32 i = 0; i < itemsCount; ++i)
    {
        Item* item = _player->GetItemByGuid(itemGUIDs[i]);

        if (!item)
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
            return;
        }

        if (sAuctionMgr->GetAItem(item->GetGUIDLow()) || !item->CanBeTraded() || item->IsNotEmptyBag() ||
            item->GetTemplate()->Flags & ITEM_PROTO_FLAG_CONJURED || item->GetUInt32Value(ITEM_FIELD_DURATION) ||
            item->GetCount() < count[i])
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
            return;
        }

        items[i] = item;
        finalCount += count[i];
    }

    if (!finalCount)
    {
        SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
        return;
    }

    for (uint32 i = 0; i < itemsCount; ++i)
    {
        Item* item = items[i];

        if (item->GetMaxStackCount() < finalCount)
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
            return;
        }
    }
   
        Item* item = items[0];

        uint32 auctionTime = uint32(etime * sWorld->getRate(RATE_AUCTION_TIME));
        AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

        uint32 deposit = sAuctionMgr->GetAuctionDeposit(auctionHouseEntry, etime, item, finalCount);
        if (!_player->HasEnoughMoney((uint64)deposit))
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_NOT_ENOUGHT_MONEY);
            return;
        }

        _player->ModifyMoney(-int32(deposit));

        AuctionEntry* AH = new AuctionEntry;
        AH->Id = sObjectMgr->GenerateAuctionID();

        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
            AH->auctioneer = 23442;
        else
            AH->auctioneer = GUID_LOPART(auctioneer);

        // Required stack size of auction matches to current item stack size, just move item to auctionhouse
        if (itemsCount == 1 && item->GetCount() == count[0])
        {
            if (GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
            {
                sLog->outCommand(GetAccountId(), "GM %s (Account: %u) create auction: %s (Entry: %u Count: %u)",
                    GetPlayerName().c_str(), GetAccountId(), item->GetTemplate()->Name1.c_str(), item->GetEntry(), item->GetCount());
            }

            AH->itemGUIDLow = item->GetGUIDLow();
            AH->itemEntry = item->GetEntry();
            AH->itemCount = item->GetCount();
            AH->owner = _player->GetGUIDLow();
            AH->startbid = bid;
            AH->bidder = 0;
            AH->bid = 0;
            AH->buyout = buyout;
            AH->expire_time = time(NULL) + auctionTime;
            AH->deposit = deposit;
            AH->auctionHouseEntry = auctionHouseEntry;

            sLog->outInfo(LOG_FILTER_NETWORKIO, "CMSG_AUCTION_SELL_ITEM: Player %s (guid %d) is selling item %s entry %u (guid %d) "
                "to auctioneer %u with count %u with initial bid " UI64FMTD " with buyout " UI64FMTD " and with time %u (in sec) in auctionhouse %u",
                _player->GetName().c_str(), _player->GetGUIDLow(), item->GetTemplate()->Name1.c_str(), item->GetEntry(), item->GetGUIDLow(),
                AH->auctioneer, item->GetCount(), bid, buyout, auctionTime, AH->GetHouseId());
            sAuctionMgr->AddAItem(item);
            auctionHouse->AddAuction(AH);

            _player->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);

            SQLTransaction trans = CharacterDatabase.BeginTransaction();
            item->DeleteFromInventoryDB(trans);
            item->SaveToDB(trans);
            AH->SaveToDB(trans);
            _player->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            SendAuctionCommandResult(AH, AUCTION_SELL_ITEM, ERR_AUCTION_OK);

            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION, 1);
            return;
        }
        else // Required stack size of auction does not match to current item stack size, clone item and set correct stack size
        {
            Item* newItem = item->CloneItem(finalCount, _player);
            if (!newItem)
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "CMSG_AUCTION_SELL_ITEM: Could not create clone of item %u", item->GetEntry());
                SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
                return;
            }

            if (GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
            {
                sLog->outCommand(GetAccountId(), "GM %s (Account: %u) create auction: %s (Entry: %u Count: %u)",
                    GetPlayerName().c_str(), GetAccountId(), newItem->GetTemplate()->Name1.c_str(), newItem->GetEntry(), newItem->GetCount());
            }

            AH->itemGUIDLow = newItem->GetGUIDLow();
            AH->itemEntry = newItem->GetEntry();
            AH->itemCount = newItem->GetCount();
            AH->owner = _player->GetGUIDLow();
            AH->startbid = bid;
            AH->bidder = 0;
            AH->bid = 0;
            AH->buyout = buyout;
            AH->expire_time = time(NULL) + auctionTime;
            AH->deposit = deposit;
            AH->auctionHouseEntry = auctionHouseEntry;

            sLog->outInfo(LOG_FILTER_NETWORKIO, "CMSG_AUCTION_SELL_ITEM: Player %s (guid %d) is selling item %s entry %u (guid %d) to "
                 "auctioneer %u with count %u with initial bid " UI64FMTD " with buyout " UI64FMTD " and with time %u (in sec) in auctionhouse %u",
                 _player->GetName().c_str(), _player->GetGUIDLow(), newItem->GetTemplate()->Name1.c_str(), newItem->GetEntry(),
                 newItem->GetGUIDLow(), AH->auctioneer, newItem->GetCount(), bid, buyout, auctionTime, AH->GetHouseId());
            sAuctionMgr->AddAItem(newItem);
            auctionHouse->AddAuction(AH);

            for (uint32 j = 0; j < itemsCount; ++j)
            {
                Item* item2 = items[j];

                // Item stack count equals required count, ready to delete item - cloned item will be used for auction
                if (item2->GetCount() == count[j])
                {
                    _player->MoveItemFromInventory(item2->GetBagSlot(), item2->GetSlot(), true);

                    SQLTransaction trans = CharacterDatabase.BeginTransaction();
                    item2->DeleteFromInventoryDB(trans);
                    item2->DeleteFromDB(trans);
                    CharacterDatabase.CommitTransaction(trans);
                }
                else // Item stack count is bigger than required count, update item stack count and save to database - cloned item will be used for auction
                {
                    item2->SetCount(item2->GetCount() - count[j]);
                    item2->SetState(ITEM_CHANGED, _player);
                    _player->ItemRemovedQuestCheck(item2->GetEntry(), count[j]);
                    item2->SendUpdateToPlayer(_player);

                    SQLTransaction trans = CharacterDatabase.BeginTransaction();
                    item2->SaveToDB(trans);
                    CharacterDatabase.CommitTransaction(trans);
                }
            }

            SQLTransaction trans = CharacterDatabase.BeginTransaction();
            newItem->SaveToDB(trans);
            AH->SaveToDB(trans);
            _player->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            SendAuctionCommandResult(AH, AUCTION_SELL_ITEM, ERR_AUCTION_OK);

            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION, 1);
            
        }
    
	_player->ModifyMoney(-int32(deposit));

    WorldPacket data2(SMSG_AUCTION_OWNER_LIST_RESULT, (4+4+4));
    data2 << uint32(0);                                     // amount place holder

    uint32 count2 = 0;
    uint32 totalcount2 = 0;

    auctionHouse->BuildListOwnerItems(data2, _player, count2, totalcount2);
    data2.put<uint32>(0, count2);
    data2 << uint32(totalcount2);
    data2 << uint32(0);
    SendPacket(&data2);
}

// this function is called when client bids or buys out auction
void WorldSession::HandleAuctionPlaceBid(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_PLACE_BID");

    ObjectGuid auctioneer;
    uint32 auctionId;
    uint64 price;

    recvData >> auctionId;
    recvData >> price;

    auctioneer[6] = recvData.ReadBit();
    auctioneer[7] = recvData.ReadBit();
    auctioneer[3] = recvData.ReadBit();
    auctioneer[5] = recvData.ReadBit();
    auctioneer[4] = recvData.ReadBit();
    auctioneer[1] = recvData.ReadBit();
    auctioneer[0] = recvData.ReadBit();
    auctioneer[2] = recvData.ReadBit();
    recvData.ReadByteSeq(auctioneer[2]);
    recvData.ReadByteSeq(auctioneer[5]);
    recvData.ReadByteSeq(auctioneer[1]);
    recvData.ReadByteSeq(auctioneer[3]);
    recvData.ReadByteSeq(auctioneer[4]);
    recvData.ReadByteSeq(auctioneer[7]);
    recvData.ReadByteSeq(auctioneer[0]);
    recvData.ReadByteSeq(auctioneer[6]);

    if (!auctionId || !price)
        return;                                             // check for cheaters

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionPlaceBid - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(auctioneer)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    AuctionEntry* auction = auctionHouse->GetAuction(auctionId);
    Player* player = GetPlayer();

    if (!auction || auction->owner == player->GetGUIDLow())
    {
        //you cannot bid your own auction:
        SendAuctionCommandResult(NULL, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
        return;
    }

    // impossible have online own another character (use this for speedup check in case online owner)
    /*Player* auction_owner = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(auction->owner, 0, HIGHGUID_PLAYER));
    if (!auction_owner && sObjectMgr->GetPlayerAccountIdByGUID(MAKE_NEW_GUID(auction->owner, 0, HIGHGUID_PLAYER)) == player->GetSession()->GetAccountId())
    {
        //you cannot bid your another character auction:
        SendAuctionCommandResult(NULL, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
        return;
    }*/

    // cheating
    if (price <= auction->bid || price < auction->startbid)
        return;

    // price too low for next bid if not buyout
    if ((price < auction->buyout || auction->buyout == 0) &&
        price < auction->bid + auction->GetAuctionOutBid())
    {
        // client already test it but just in case ...
        SendAuctionCommandResult(auction, AUCTION_PLACE_BID, ERR_AUCTION_HIGHER_BID);
        return;
    }

    if (!player->HasEnoughMoney(price))
    {
        // client already test it but just in case ...
        SendAuctionCommandResult(auction, AUCTION_PLACE_BID, ERR_AUCTION_NOT_ENOUGHT_MONEY);
        return;
    }

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    if (price < auction->buyout || auction->buyout == 0)
    {
        if (auction->bidder > 0)
        {
            if (auction->bidder == player->GetGUIDLow())
                player->ModifyMoney(-int64(price - auction->bid));
            else
            {
                // mail to last bidder and return money
                sAuctionMgr->SendAuctionOutbiddedMail(auction, price, GetPlayer(), trans);
                player->ModifyMoney(-int64(price));
            }
        }
        else
            player->ModifyMoney(-int64(price));

        auction->bidder = player->GetGUIDLow();
        auction->bid = price;
        GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID, price);

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_AUCTION_BID);
        stmt->setUInt32(0, auction->bidder);
        stmt->setUInt32(1, auction->bid);
        stmt->setUInt32(2, auction->Id);
        trans->Append(stmt);

        SendAuctionCommandResult(auction, AUCTION_PLACE_BID, ERR_AUCTION_OK);
    }
    else
    {
        //buyout:
        if (player->GetGUIDLow() == auction->bidder)
            player->ModifyMoney(-int64(auction->buyout - auction->bid));
        else
        {
            player->ModifyMoney(-int64(auction->buyout));
            if (auction->bidder)                          //buyout for bidded auction ..
                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, GetPlayer(), trans);
        }
        auction->bidder = player->GetGUIDLow();
        auction->bid = auction->buyout;
        GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID, auction->buyout);

        //- Mails must be under transaction control too to prevent data loss
        sAuctionMgr->SendAuctionSalePendingMail(auction, trans);
        sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
        sAuctionMgr->SendAuctionWonMail(auction, trans);

        SendAuctionCommandResult(auction, AUCTION_PLACE_BID, ERR_AUCTION_OK);

        auction->DeleteFromDB(trans);

        uint32 itemEntry = auction->itemEntry;
        sAuctionMgr->RemoveAItem(auction->itemGUIDLow);
        auctionHouse->RemoveAuction(auction, itemEntry);
    }
    player->SaveInventoryAndGoldToDB(trans);
    CharacterDatabase.CommitTransaction(trans);

	WorldPacket data(SMSG_AUCTION_BIDDER_LIST_RESULT, (4+4+4));
    data << uint32(0);                                     //add 0 as count
    uint32 count = 0;
    uint32 totalcount = 0;

    auctionHouse->BuildListBidderItems(data, player, count, totalcount);
    data.put<uint32>(0, count);                           // add count to placeholder
    data << totalcount;
    data << uint32(300);                                  //unk 2.3.0
    SendPacket(&data);
}

//this void is called when auction_owner cancels his auction
void WorldSession::HandleAuctionRemoveItem(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_REMOVE_ITEM");

    ObjectGuid auctioneer;
    uint32 auctionId;

    recvData >> auctionId;
    auctioneer[1] = recvData.ReadBit();
    auctioneer[5] = recvData.ReadBit();
    auctioneer[4] = recvData.ReadBit();
    auctioneer[2] = recvData.ReadBit();
    auctioneer[7] = recvData.ReadBit();
    auctioneer[6] = recvData.ReadBit();
    auctioneer[0] = recvData.ReadBit();
    auctioneer[3] = recvData.ReadBit();
    recvData.ReadByteSeq(auctioneer[3]);
    recvData.ReadByteSeq(auctioneer[5]);
    recvData.ReadByteSeq(auctioneer[2]);
    recvData.ReadByteSeq(auctioneer[7]);
    recvData.ReadByteSeq(auctioneer[1]);
    recvData.ReadByteSeq(auctioneer[0]);
    recvData.ReadByteSeq(auctioneer[6]);
    recvData.ReadByteSeq(auctioneer[4]);

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionRemoveItem - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(auctioneer)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    AuctionEntry* auction = auctionHouse->GetAuction(auctionId);
    Player* player = GetPlayer();

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    if (auction && auction->owner == player->GetGUIDLow())
    {
        Item* pItem = sAuctionMgr->GetAItem(auction->itemGUIDLow);
        if (pItem)
        {
            if (auction->bidder > 0)                        // If we have a bidder, we have to send him the money he paid
            {
                uint32 auctionCut = auction->GetAuctionCut();
                if (!player->HasEnoughMoney((uint64)auctionCut))          //player doesn't have enough money, maybe message needed
                    return;
                sAuctionMgr->SendAuctionCancelledToBidderMail(auction, trans, pItem);
                player->ModifyMoney(-int64(auctionCut));
            }

            // item will deleted or added to received mail list
            MailDraft(auction->BuildAuctionMailSubject(AUCTION_CANCELED), AuctionEntry::BuildAuctionMailBody(0, 0, auction->buyout, auction->deposit, 0))
                .AddItem(pItem)
                .SendMailTo(trans, player, auction, MAIL_CHECK_MASK_COPIED);
        }
        else
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Auction id: %u got non existing item (item guid : %u)!", auction->Id, auction->itemGUIDLow);
            SendAuctionCommandResult(NULL, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
            return;
        }
    }
    else
    {
        SendAuctionCommandResult(NULL, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
        //this code isn't possible ... maybe there should be assert
        sLog->outError(LOG_FILTER_NETWORKIO, "CHEATER: %u tried to cancel auction (id: %u) of another player or auction is NULL", player->GetGUIDLow(), auctionId);
        return;
    }

    //inform player, that auction is removed
    SendAuctionCommandResult(auction, AUCTION_CANCEL, ERR_AUCTION_OK);

    // Now remove the auction

    player->SaveInventoryAndGoldToDB(trans);
    auction->DeleteFromDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    uint32 itemEntry = auction->itemEntry;
    sAuctionMgr->RemoveAItem(auction->itemGUIDLow);
    auctionHouse->RemoveAuction(auction, itemEntry);

	WorldPacket data(SMSG_AUCTION_OWNER_LIST_RESULT, (4+4+4));
    data << uint32(0);                                     // amount place holder

    uint32 count = 0;
    uint32 totalcount = 0;

    auctionHouse->BuildListOwnerItems(data, _player, count, totalcount);
    data.put<uint32>(0, count);
    data << uint32(totalcount);
    data << uint32(0);
    SendPacket(&data);
}

//called when player lists his bids
void WorldSession::HandleAuctionListBidderItems(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_LIST_BIDDER_ITEMS");

    ObjectGuid guid;                                            //NPC guid
    uint32 listfrom;                                        //page of auctions
    uint8 outbiddedCount;                                  //count of outbidded auctions
    std::vector<uint32> outbiddedAuctionIdList;

    recvData >> listfrom;                                  // not used in fact (this list not have page control in client)
    outbiddedCount = recvData.ReadBits(7);
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    for(uint8 i = 0 ; i < outbiddedCount ; i++)
    {
        uint32 outBiddedAuctionId;
        recvData >> outBiddedAuctionId;
        outbiddedAuctionIdList.push_back(outBiddedAuctionId);
    }
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListBidderItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        recvData.rfinish();
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    WorldPacket data(SMSG_AUCTION_BIDDER_LIST_RESULT, (4+4+4));
    Player* player = GetPlayer();
    data << uint32(0);                                     //add 0 as count
    uint32 count = 0;
    uint32 totalcount = 0;
    for(int i = 0 ; i < outbiddedCount ; i++)                            //add all data, which client requires
    {
        uint32 outbiddedAuctionId = outbiddedAuctionIdList[i];
        AuctionEntry* auction = auctionHouse->GetAuction(outbiddedAuctionId);
        if (auction && auction->BuildAuctionInfo(data))
        {
            ++totalcount;
            ++count;
        }
    }

    auctionHouse->BuildListBidderItems(data, player, count, totalcount);
    data.put<uint32>(0, count);                           // add count to placeholder
    data << totalcount;
    data << uint32(300);                                  //unk 2.3.0
    SendPacket(&data);
}

//this void sends player info about his auctions
void WorldSession::HandleAuctionListOwnerItems(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_LIST_OWNER_ITEMS");

    uint32 listfrom;
    ObjectGuid guid;

    recvData >> listfrom;
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[1]);

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListOwnerItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

   WorldPacket data(SMSG_AUCTION_OWNER_LIST_RESULT, (4+4+4));
    data << uint32(0);                                     // amount place holder

    uint32 count = 0;
    uint32 totalcount = 0;

    auctionHouse->BuildListOwnerItems(data, _player, count, totalcount);
    data.put<uint32>(0, count);
    data << uint32(totalcount);
    data << uint32(0);
    SendPacket(&data);
}

//this void is called when player clicks on search button
void WorldSession::HandleAuctionListItems(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_LIST_ITEMS");

    uint32 auctionMainCategory, auctionSubCategory, auctionSlotID, quality, listfrom, dataLen;
    uint8 unk7, levelmin, levelmax;
    std::string searchedname;
    ObjectGuid guid;
    uint8 searchednameLength, usable;
    std::list<uint8> dataList;


    recvData >> unk7;
    recvData >> quality;
    recvData >> auctionSubCategory;
    recvData >> auctionSlotID;
    recvData >> auctionMainCategory;
    recvData >> levelmax;
    recvData >> levelmin;
    recvData >> listfrom;
    recvData >> dataLen;
    for(uint32 i = 0 ; i < dataLen ; i++)
    {
        uint8 dataCur;
        recvData >> dataCur;
        dataList.push_back(dataCur);
    }
    searchednameLength = recvData.ReadBits(8);
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    usable = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);

    searchedname = recvData.ReadString(searchednameLength);

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    //sLog->outDebug(LOG_FILTER_GENERAL, "Auctionhouse search (GUID: %u TypeId: %u)",, list from: %u, searchedname: %s, levelmin: %u, levelmax: %u, auctionSlotID: %u, auctionMainCategory: %u, auctionSubCategory: %u, quality: %u, usable: %u",
    //  GUID_LOPART(guid), GuidHigh2TypeId(GUID_HIPART(guid)), listfrom, searchedname.c_str(), levelmin, levelmax, auctionSlotID, auctionMainCategory, auctionSubCategory, quality, usable);

    WorldPacket data(SMSG_AUCTION_LIST_RESULT, (4+4+4));
    uint32 count = 0;
    uint32 totalcount = 0;
    data << uint32(0);

    // converting string that we try to find to lower case
    std::wstring wsearchedname;
    if (!Utf8toWStr(searchedname, wsearchedname))
        return;

    wstrToLower(wsearchedname);

    auctionHouse->BuildListAuctionItems(data, _player,
        wsearchedname, listfrom, levelmin, levelmax, usable,
        auctionSlotID, auctionMainCategory, auctionSubCategory, quality,
        count, totalcount);

    data.put<uint32>(0, count);
    data << uint32(totalcount);
    data << uint32(300);                                  //unk 2.3.0
    SendPacket(&data);
}

void WorldSession::HandleAuctionListPendingSales(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_AUCTION_LIST_PENDING_SALES");

    //recvData.read_skip<uint64>();
	    
    WorldPacket data(SMSG_AUCTION_LIST_PENDING_SALES, 4);
    data << uint32(0);                                     // amount place holder

    uint32 count = 0;
    uint32 totalcount = 0;
    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(GetPlayer()->getFaction());
    auctionHouse->BuildListOwnerItems(data, _player, count, totalcount);
    data.put<uint32>(0, count);
    data << uint32(totalcount);
    data << uint32(0);
    SendPacket(&data);
}
