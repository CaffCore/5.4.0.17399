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

#include "QuestDef.h"
#include "GossipDef.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Formulas.h"

GossipMenu::GossipMenu()
{
    _menuId = 0;
}

GossipMenu::~GossipMenu()
{
    ClearMenu();
}

void GossipMenu::AddMenuItem(int32 menuItemId, uint8 icon, std::string const& message, uint32 sender, uint32 action, std::string const& boxMessage, uint32 boxMoney, bool coded /*= false*/)
{
    ASSERT(_menuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    // Find a free new id - script case
    if (menuItemId == -1)
    {
        menuItemId = 0;
        if (!_menuItems.empty())
        {
            for (GossipMenuItemContainer::const_iterator itr = _menuItems.begin(); itr != _menuItems.end(); ++itr)
            {
                if (int32(itr->first) > menuItemId)
                    break;

                menuItemId = itr->first + 1;
            }
        }
    }

    GossipMenuItem& menuItem = _menuItems[menuItemId];

    menuItem.MenuItemIcon    = icon;
    menuItem.Message         = message;
    menuItem.IsCoded         = coded;
    menuItem.Sender          = sender;
    menuItem.OptionType      = action;
    menuItem.BoxMessage      = boxMessage;
    menuItem.BoxMoney        = boxMoney;
}

void GossipMenu::AddGossipMenuItemData(uint32 menuItemId, uint32 gossipActionMenuId, uint32 gossipActionPoi)
{
    GossipMenuItemData& itemData = _menuItemData[menuItemId];

    itemData.GossipActionMenuId  = gossipActionMenuId;
    itemData.GossipActionPoi     = gossipActionPoi;
}

uint32 GossipMenu::GetMenuItemSender(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.Sender;
}

uint32 GossipMenu::GetMenuItemAction(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.OptionType;
}

bool GossipMenu::IsMenuItemCoded(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return false;

    return itr->second.IsCoded;
}

void GossipMenu::ClearMenu()
{
    _menuItems.clear();
    _menuItemData.clear();
}

PlayerMenu::PlayerMenu(WorldSession* session) : _session(session)
{
}

PlayerMenu::~PlayerMenu()
{
    ClearMenus();
}

void PlayerMenu::ClearMenus()
{
    _gossipMenu.ClearMenu();
    _questMenu.ClearMenu();
}

void PlayerMenu::SendGossipMenu(uint32 titleTextId, uint64 objectGUID) const
{
    WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);         // guess size
    ObjectGuid guid = objectGUID;

    data << uint32(_gossipMenu.GetMenuId());
    data << uint32(0); // unk seems always 0
    data << uint32(titleTextId);

    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);
    data.WriteBits(_gossipMenu.GetMenuItemCount(), 20);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);

    for (GossipMenuItemContainer::const_iterator itr = _gossipMenu.GetMenuItems().begin(); itr != _gossipMenu.GetMenuItems().end(); ++itr)
    {
        GossipMenuItem const& item = itr->second;
        data.WriteBits(item.Message.length(), 12);
        data.WriteBits(item.BoxMessage.length(), 12);
    }

    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBits(_questMenu.GetMenuItemCount(), 19);

    for (uint8 i = 0; i < _questMenu.GetMenuItemCount(); ++i)
    {
        QuestMenuItem const& item = _questMenu.GetItem(i);
        uint32 questID = item.QuestId;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questID);
        data.WriteBits(quest->GetTitle().length(), 9);
        data.WriteBit(0); //Change Icon
    }

    data.WriteBit(guid[5]);
    data.FlushBits();

    for (uint8 i = 0; i < _questMenu.GetMenuItemCount(); ++i)
    {
        QuestMenuItem const& item = _questMenu.GetItem(i);
        uint32 questID = item.QuestId;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questID);
        data << uint32(0); // 3.3.3 changes icon: blue question or yellow exclamation
        data << uint32(item.QuestIcon);
        data.WriteString(quest->GetTitle());
        data << uint32(quest->GetFlags());
        data << uint32(quest->GetQuestLevel());
        data << uint32(questID);
    }

    for (GossipMenuItemContainer::const_iterator itr = _gossipMenu.GetMenuItems().begin(); itr != _gossipMenu.GetMenuItems().end(); ++itr)
    {
        //Gossip menu
        GossipMenuItem const& item = itr->second;
        data << uint32(item.BoxMoney);
        data << uint32(itr->first);
        data.WriteString(item.BoxMessage);
        data << uint8(item.IsCoded);
        data.WriteString(item.Message);
        data << uint8(item.MenuItemIcon); 
    }

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);

    _session->SendPacket(&data);
}

void PlayerMenu::SendCloseGossip() const
{
    WorldPacket data(SMSG_GOSSIP_COMPLETE, 0);
    _session->SendPacket(&data);
}

void PlayerMenu::SendPointOfInterest(uint32 poiId) const
{
    PointOfInterest const* poi = sObjectMgr->GetPointOfInterest(poiId);
    if (!poi)
    {
        sLog->outError(LOG_FILTER_SQL, "Request to send non-existing POI (Id: %u), ignored.", poiId);
        return;
    }

    std::string iconText = poi->icon_name;
    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
        if (PointOfInterestLocale const* localeData = sObjectMgr->GetPointOfInterestLocale(poiId))
            ObjectMgr::GetLocaleString(localeData->IconName, locale, iconText);

    WorldPacket data(SMSG_GOSSIP_POI, 4 + 4 + 4 + 4 + 4 + 10);  // guess size
    data << uint32(poi->flags);
    data << float(poi->x);
    data << float(poi->y);
    data << uint32(poi->icon);
    data << uint32(poi->data);
    data << iconText;

    _session->SendPacket(&data);
}

/*********************************************************/
/***                    QUEST SYSTEM                   ***/
/*********************************************************/

QuestMenu::QuestMenu()
{
    _questMenuItems.reserve(16);                                   // can be set for max from most often sizes to speedup push_back and less memory use
}

QuestMenu::~QuestMenu()
{
    ClearMenu();
}

void QuestMenu::AddMenuItem(uint32 QuestId, uint8 Icon)
{
    if (!sObjectMgr->GetQuestTemplate(QuestId))
        return;

    ASSERT(_questMenuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    QuestMenuItem questMenuItem;

    questMenuItem.QuestId        = QuestId;
    questMenuItem.QuestIcon      = Icon;

    _questMenuItems.push_back(questMenuItem);
}

bool QuestMenu::HasItem(uint32 questId) const
{
    for (QuestMenuItemList::const_iterator i = _questMenuItems.begin(); i != _questMenuItems.end(); ++i)
        if (i->QuestId == questId)
            return true;

    return false;
}

void QuestMenu::ClearMenu()
{
    _questMenuItems.clear();
}

void PlayerMenu::SendQuestGiverQuestList(QEmote eEmote, const std::string& Title, uint64 npcGUID)
{
    WorldPacket data(SMSG_QUESTGIVER_QUEST_LIST, 100);    // guess size
    data << uint64(npcGUID);
    data << Title;
    data << uint32(eEmote._Delay);                         // player emote
    data << uint32(eEmote._Emote);                         // NPC emote

    size_t count_pos = data.wpos();
    data << uint8 (_questMenu.GetMenuItemCount());
    uint32 count = 0;

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (; count < _questMenu.GetMenuItemCount(); ++count)
    {
        QuestMenuItem const& qmi = _questMenu.GetItem(count);

        uint32 questID = qmi.QuestId;

        if (Quest const* quest = sObjectMgr->GetQuestTemplate(questID))
        {
            std::string title = quest->GetTitle();

            int32 locale = _session->GetSessionDbLocaleIndex();
            if (locale >= 0)
                if (QuestLocale const* localeData = sObjectMgr->GetQuestLocale(questID))
                    ObjectMgr::GetLocaleString(localeData->Title, locale, title);

            if (questLevelInTitle)
                AddQuestLevelToTitle(title, quest->GetQuestLevel());

            data << uint32(questID);
            data << uint32(qmi.QuestIcon);
            data << int32(quest->GetQuestLevel());
            data << uint32(quest->GetFlags());             // 3.3.3 quest flags
            data << uint8(0);                               // 3.3.3 changes icon: blue question or yellow exclamation
            data << title;
        }
    }

    data.put<uint8>(count_pos, count);
    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST NPC Guid=%u", GUID_LOPART(npcGUID));
}

void PlayerMenu::SendQuestGiverStatus(uint32 questStatus, uint64 npcGUID) const
{
    WorldPacket data(SMSG_QUESTGIVER_STATUS, 8 + 4);
    data << uint64(npcGUID);
    data << uint32(questStatus);

    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUESTGIVER_STATUS NPC Guid=%u, status=%u", GUID_LOPART(npcGUID), questStatus);
}

void PlayerMenu::SendQuestGiverQuestDetails(Quest const* quest, uint64 npcGUID, bool activateAccept) const
{
    std::string questTitle           = quest->GetTitle();
    std::string questDetails         = quest->GetDetails();
    std::string questObjectives      = quest->GetObjectives();
    std::string questEndText         = quest->GetEndText();
    std::string questGiverTextWindow = quest->GetQuestGiverTextWindow();
    std::string questGiverTargetName = quest->GetQuestGiverTargetName();
    std::string questTurnTextWindow  = quest->GetQuestTurnTextWindow();
    std::string questTurnTargetName  = quest->GetQuestTurnTargetName();

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->Title, locale, questTitle);
            ObjectMgr::GetLocaleString(localeData->Details, locale, questDetails);
            ObjectMgr::GetLocaleString(localeData->Objectives, locale, questObjectives);
            ObjectMgr::GetLocaleString(localeData->EndText, locale, questEndText);
            ObjectMgr::GetLocaleString(localeData->QuestGiverTextWindow, locale, questGiverTextWindow);
            ObjectMgr::GetLocaleString(localeData->QuestGiverTargetName, locale, questGiverTargetName);
            ObjectMgr::GetLocaleString(localeData->QuestTurnTextWindow, locale, questTurnTextWindow);
            ObjectMgr::GetLocaleString(localeData->QuestTurnTargetName, locale, questTurnTargetName);
        }
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

	size_t l_RewardItemCount = 0;
    for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        if (quest->RewardItemId[i])
            l_RewardItemCount++;

    WorldPacket data(SMSG_QUESTGIVER_QUEST_DETAILS, 100);   // guess size
	data << uint32(0);
    data << uint32(0);

    for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i) 
    {
        data << uint32(quest->RewardFactionId[i]);
        data << uint32(quest->RewardFactionValueIdOverride[i]);
        data << int32(quest->RewardFactionValueId[i]);
    }

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[3]);
    data << uint32(0);
    data << uint32(quest->GetRewChoiceItemsCount());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[3]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[2]);

    data << uint32(quest->XPValue(_session->GetPlayer()) * sWorld->getRate(RATE_XP_QUEST));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[4]);
    data << uint32(quest->GetQuestId());

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[3]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1])->DisplayInfoID : 0));
    data << uint32(0);

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[1]);
    data << uint32(quest->GetQuestGiverPortrait());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2])->DisplayInfoID : 0));
    data << uint32(l_RewardItemCount);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[1])->DisplayInfoID : 0));
    data << uint32(quest->GetRewOrReqMoney());
    data << uint32(quest->GetFlags());
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[0]);

    for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
    {
        data << uint32(quest->RewardCurrencyId[i]);
        data << uint32(quest->RewardCurrencyCount[i]);
    }

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[5]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[0]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[4]);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[0])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[3]);
    data << uint32(0);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[5]);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[0]);

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[2])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[3])->DisplayInfoID : 0));
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5])->DisplayInfoID : 0));

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[0]);

    ObjectGuid l_Guid1 = npcGUID;
    ObjectGuid l_Guid2 = npcGUID; //_session->GetPlayer()->GetDivider();

    uint32 l_CreOrGoObjCount = 0;
    uint8 l_EmoteCount = 0;

    for (uint32 i = 0  ; i < QUEST_OBJECTIVES_COUNT ; ++i) 
        if(quest->RequiredNpcOrGoCount[i])
            l_CreOrGoObjCount++;

    for (uint8 i = 0 ; i < QUEST_EMOTE_COUNT ; ++i)
        if (quest->DetailsEmote[i])
            l_EmoteCount++;

    data.WriteBit(l_Guid1[4]);
    data.WriteBit(l_Guid2[0]);  
    data.WriteBit(l_Guid2[3]);
    data.WriteBits(questGiverTextWindow.length(), 10);
    data.WriteBit(l_Guid1[2]); 
    data.WriteBit(l_Guid1[1]); 
    data.WriteBits(questTurnTextWindow.length(), 10);
    data.WriteBits(questDetails.length(), 12);
    data.WriteBit(l_Guid1[5]); 
    data.WriteBit(0);
    data.WriteBit(l_Guid1[0]); 
    data.WriteBits(l_EmoteCount, 21);
    data.WriteBits(0, 22);
    data.WriteBits(questObjectives.length(), 12);
    data.WriteBits(l_CreOrGoObjCount + QUEST_ITEM_OBJECTIVES_COUNT, 20);
    data.WriteBits(questTurnTargetName.length(), 8);
    data.WriteBit(l_Guid2[5]); 
    data.WriteBit(l_Guid2[4]);
    data.WriteBit(l_Guid2[2]);
    data.WriteBit(l_Guid2[7]);
    data.WriteBits(questGiverTargetName.length(), 8);
    data.WriteBits(questTitle.length(), 9);
    data.WriteBit(l_Guid2[6]);
    data.WriteBit(0);
    data.WriteBit(l_Guid1[3]);  
    data.WriteBit(l_Guid2[1]);
    data.WriteBit(l_Guid1[7]);
    data.WriteBit(l_Guid1[6]);  
    data.WriteBit(0);

    data.FlushBits();

    for (uint8 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (!quest->DetailsEmote[i])
            continue;

        data << uint32(quest->DetailsEmoteDelay[i]);       // DetailsEmoteDelay (in ms)
        data << uint32(quest->DetailsEmote[i]);
    }

    data.WriteByteSeq(l_Guid2[4]);
    data.WriteByteSeq(l_Guid2[5]);

    data.WriteString(questTurnTextWindow);
    data.WriteString(questGiverTextWindow);

    data.WriteByteSeq(l_Guid2[1]);
    data.WriteByteSeq(l_Guid1[0]);

    for (uint32 i = 0  ; i < QUEST_OBJECTIVES_COUNT ; ++i) 
    {
        if (!quest->RequiredNpcOrGo[i])
            continue;

        data << uint8(quest->RequiredNpcOrGo > 0 ? 0 : 2);   
        data << uint32(0); 
        if (quest->RequiredNpcOrGo[i] < 0)
            data << uint32((quest->RequiredNpcOrGo[i] * (-1))/* | 0x80000000*/);    // client expects gameobject template id in form (id|0x80000000)
        else
            data << uint32(quest->RequiredNpcOrGo[i]);  
        data << uint32(quest->RequiredNpcOrGoCount[i]);
    }
    for (uint32 i = 0  ; i < QUEST_ITEM_OBJECTIVES_COUNT ; ++i) 
    {
        data << uint8(1); 
        data << uint32(0); 
        data << uint32(quest->RequiredItemId[i]);  
        data << uint32(quest->RequiredItemCount[i]);
    }

    data.WriteByteSeq(l_Guid2[0]);
    data.WriteByteSeq(l_Guid1[4]);
    data.WriteByteSeq(l_Guid1[3]);
    data.WriteString(questTurnTargetName);
    data.WriteString(questGiverTargetName);
    data.WriteByteSeq(l_Guid2[2]);

  //  for (uint32 i = 0  ; i < QUEST_OBJECTIVES_COUNT ; ++i) 
  //  {
		//if(quest->RequiredNpcOrGoCount[i])
		//{
		//	if (quest->RequiredNpcOrGo[i] < 0)
		//		data << uint32((quest->RequiredNpcOrGo[i] * (-1)) | 0x80000000);    // client expects gameobject template id in form (id|0x80000000)
		//	else
		//		data << uint32(quest->RequiredNpcOrGo[i]);
		//}
  //  }

    data.WriteByteSeq(l_Guid2[6]);
    data.WriteString(questTitle);
    data.WriteByteSeq(l_Guid1[7]);
    data.WriteString(questObjectives);
    data.WriteByteSeq(l_Guid1[2]);
    data.WriteByteSeq(l_Guid2[3]);
    data.WriteByteSeq(l_Guid1[6]);
    data.WriteString(questDetails);
    data.WriteByteSeq(l_Guid1[5]);
    data.WriteByteSeq(l_Guid2[7]);
    data.WriteByteSeq(l_Guid1[1]);
    
    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUEST_QUERY_RESPONSE questid=%u", quest->GetQuestId());
}

void PlayerMenu::SendQuestQueryResponse(Quest const* quest) const
{
    std::string questTitle = quest->GetTitle();
    std::string questDetails = quest->GetDetails();
    std::string questObjectives = quest->GetObjectives();
    std::string questEndText = quest->GetEndText();
    std::string questCompletedText = quest->GetCompletedText();
    std::string questGiverTextWindow = quest->GetQuestGiverTextWindow();
    std::string questGiverTargetName = quest->GetQuestGiverTargetName();
    std::string questTurnTextWindow = quest->GetQuestTurnTextWindow();
    std::string questTurnTargetName = quest->GetQuestTurnTargetName();

    std::string questObjectiveText[QUEST_OBJECTIVES_COUNT];
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        questObjectiveText[i] = quest->ObjectiveText[i];

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->Title, locale, questTitle); //ok
            ObjectMgr::GetLocaleString(localeData->Details, locale, questDetails); // ok
            ObjectMgr::GetLocaleString(localeData->Objectives, locale, questObjectives);
            ObjectMgr::GetLocaleString(localeData->EndText, locale, questEndText);
            ObjectMgr::GetLocaleString(localeData->CompletedText, locale, questCompletedText); // ok
            ObjectMgr::GetLocaleString(localeData->QuestGiverTextWindow, locale, questGiverTextWindow); // ok
            ObjectMgr::GetLocaleString(localeData->QuestGiverTargetName, locale, questGiverTargetName); // ok
            ObjectMgr::GetLocaleString(localeData->QuestTurnTextWindow, locale, questTurnTextWindow); // ok
            ObjectMgr::GetLocaleString(localeData->QuestTurnTargetName, locale, questTurnTargetName); // ok

            for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
                ObjectMgr::GetLocaleString(localeData->ObjectiveText[i], locale, questObjectiveText[i]);
        }
    }

    uint8 l_ObjectivesCount = 0;
    for (uint32 i = 0  ; i < QUEST_OBJECTIVES_COUNT ; ++i) 
        if (quest->RequiredNpcOrGo[i] != 0)
            l_ObjectivesCount++;
    for (uint32 i = 0  ; i < QUEST_ITEM_OBJECTIVES_COUNT ; ++i) 
        if (quest->RequiredItemId[i] != 0)
            l_ObjectivesCount++;

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

    WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);       // guess size

    data.WriteBit(1);   // HasData

    data.WriteBits(questDetails.length(), 12);
    data.WriteBits(l_ObjectivesCount, 19);

    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        if (!quest->RequiredNpcOrGo[i])
            continue;

        data.WriteBits(questObjectiveText[i].size(), 8);
        data.WriteBits(0, 22);
    }
    for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
    {
        if (!quest->RequiredItemId[i])
            continue;

        data.WriteBits(0, 8);
        data.WriteBits(0, 22);
    }

    data.WriteBits(questCompletedText.length(), 11);
    data.WriteBits(questGiverTargetName.length(), 8);
    data.WriteBits(questTurnTextWindow.length(), 10);
    data.WriteBits(questObjectives.length(), 12);
    data.WriteBits(questEndText.length(), 9);
    data.WriteBits(questTurnTargetName.length(), 8);
    data.WriteBits(questGiverTextWindow.length(), 10);
    data.WriteBits(questTitle.length(), 9);

    data.FlushBits();

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[2]);
    data << int32(quest->GetRewSpellCast());
    data << float(quest->GetRewHonorMultiplier());

    for (uint32 i = 0  ; i < QUEST_OBJECTIVES_COUNT ; ++i) 
    {
        if (!quest->RequiredNpcOrGo[i])
            continue;

        data << uint32(0); 
        if (quest->RequiredNpcOrGo[i] < 0)
            data << uint32((quest->RequiredNpcOrGo[i] * (-1))/* | 0x80000000*/);    // client expects gameobject template id in form (id|0x80000000)
        else
            data << uint32(quest->RequiredNpcOrGo[i]);  
        data << uint32(0);
        data << uint32(quest->RequiredNpcOrGoCount[i]);

        data.WriteString(questObjectiveText[i]);

        data << uint8(0);
        data << uint8(quest->RequiredNpcOrGo > 0 ? 0 : 2);   
    }
    for (uint32 i = 0  ; i < QUEST_ITEM_OBJECTIVES_COUNT ; ++i) 
    {
        if (!quest->RequiredItemId[i])
            continue;

        data << uint32(0); 
        data << uint32(quest->RequiredItemId[i]);  
        data << uint32(0);
        data << uint32(quest->RequiredItemCount[i]);

        data << uint8(0);
        data << uint8(1);   
    }

    data << uint32(quest->GetSrcItemId());
    data << uint32(0);
    data << float(quest->GetPointY());
    data.WriteString(questTurnTargetName);
    data << uint32(quest->GetNextQuestInChain());
    data.WriteString(questEndText);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[2]);

    for (uint32 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
    {
        data << uint32(quest->RewardCurrencyCount[i]);
        data << uint32(quest->RewardCurrencyId[i]);
    }

    data << int32(quest->GetQuestLevel());
    data << float(quest->GetPointX());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[3]);

    for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)        
    {   
        data << int32(quest->RewardFactionValueId[i]);              // columnid+1 QuestFactionReward.dbc?
        data << int32(quest->RewardFactionValueIdOverride[i]);      // unknown usage
        data << uint32(quest->RewardFactionId[i]);                  // reward factions ids     
    }

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2])->DisplayInfoID : 0));
    data << uint32(quest->GetQuestGiverPortrait());
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[4]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[3])->DisplayInfoID : 0));
    data.WriteString(questTitle);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[0]);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0])->DisplayInfoID : 0));
    data << uint32(0);
    data << uint32(0);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[1]);
    data << uint32(quest->GetQuestId());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[1]);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[2]);
    data << uint32(0);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[1]);

    data << uint32(0);
    data << uint32(0);
    data.WriteString(questTurnTextWindow);
    data << uint32(quest->GetSoundTurnIn());
    data << uint32(quest->GetXPId());

    data << uint32(quest->GetFlags() & 0xFFFF);
    data.WriteString(questCompletedText);
    data << uint32(quest->GetMinLevel());
    data << uint32(0);
    data << uint32(0);

    data << uint32(0);
    data << uint32(quest->GetRewMoneyMaxLevel());
    data << uint32(quest->GetPointMapId());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[2])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5])->DisplayInfoID : 0));

    data << int32(quest->GetZoneOrSort());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[0]);
    data << uint32(0);
    data.WriteString(questDetails);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[0]);

    data.WriteString(questGiverTargetName);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[0]); 
    data << uint32(quest->GetQuestMethod());
    data << uint32(0);

    data << uint32(0);
    data.WriteString(questGiverTextWindow);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 :quest->GetRewOrReqMoney());
    data << uint32(quest->GetType()); 
    data.WriteString(questObjectives);

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[3]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[1])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[5]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[3]); 

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[3]);
    data << uint32(quest->GetSoundAccept());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[5]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[0])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[2]);

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[4]);

    data << uint32(quest->GetQuestId());
    
    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUEST_QUERY_RESPONSE questid=%u", quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverOfferReward(Quest const* quest, uint64 npcGUID, bool enableNext) const
{
    std::string questTitle = quest->GetTitle();
    std::string questOfferRewardText = quest->GetOfferRewardText();
    std::string questGiverTextWindow = quest->GetQuestGiverTextWindow();
    std::string questGiverTargetName = quest->GetQuestGiverTargetName();
    std::string questTurnTextWindow = quest->GetQuestTurnTextWindow();
    std::string questTurnTargetName = quest->GetQuestTurnTargetName();

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->Title, locale, questTitle);
            ObjectMgr::GetLocaleString(localeData->OfferRewardText, locale, questOfferRewardText);
            ObjectMgr::GetLocaleString(localeData->QuestGiverTextWindow, locale, questGiverTextWindow);
            ObjectMgr::GetLocaleString(localeData->QuestGiverTargetName, locale, questGiverTargetName);
            ObjectMgr::GetLocaleString(localeData->QuestTurnTextWindow, locale, questTurnTextWindow);
            ObjectMgr::GetLocaleString(localeData->QuestTurnTargetName, locale, questTurnTargetName);
        }
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

	size_t l_RewardItemCount = 0;
    for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        if (quest->RewardItemId[i])
            l_RewardItemCount++;

    uint32 emoteCount = 0;
    for (uint8 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (quest->OfferRewardEmote[i] <= 0)
            break;
        ++emoteCount;
    }

    WorldPacket data(SMSG_QUESTGIVER_OFFER_REWARD, 50);     // guess size

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[2])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[0])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[0]);

    for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
    {
        data << uint32(quest->RewardCurrencyId[i]);
        data << uint32(quest->RewardCurrencyCount[i]);
    }

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[0]);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[4]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[4])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[2]);

    for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i) 
    {
        data << int32(quest->RewardFactionValueId[i]);
        data << uint32(quest->RewardFactionValueIdOverride[i]);
        data << uint32(quest->RewardFactionId[i]);
    }

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[3])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[1])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[3]);
    data << uint32(l_RewardItemCount);

    data << uint32(quest->XPValue(_session->GetPlayer()) * sWorld->getRate(RATE_XP_QUEST));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[3]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[0]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[0])->DisplayInfoID : 0));

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[5]);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[3]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[1])->DisplayInfoID : 0));

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[3]);
    data << uint32(0);
    data << uint32(quest->GetFlags());
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[5]);

    data << uint32(0);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemCount[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[3])->DisplayInfoID : 0));

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[1]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[4]);
    data << uint32(0);

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardItemId[2]) ? sObjectMgr->GetItemTemplate(quest->RewardItemId[2])->DisplayInfoID : 0));
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : (sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5]) ? sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[5])->DisplayInfoID : 0));
    data << uint32(quest->GetRewChoiceItemsCount());

    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[2]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[1]);
    data << uint32(quest->GetRewOrReqMoney());

    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardChoiceItemId[0]);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemId[0]);
    data << uint32(0);
    data << uint32(quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS) ? 0 : quest->RewardItemIdCount[1]);

    data << uint32(0);
    data << uint32(quest->GetQuestId());

    ObjectGuid l_Guid1 = npcGUID;

    data.WriteBit(l_Guid1[4]);
    data.WriteBits(questTitle.length(), 9);
    data.WriteBits(questTurnTextWindow.length(), 10);
    data.WriteBits(questGiverTextWindow.length(), 10);
    data.WriteBits(emoteCount, 21);
    data.WriteBit(0);
    data.WriteBit(l_Guid1[6]); 
    data.WriteBits(questTurnTargetName.length(), 8);
    data.WriteBits(questGiverTargetName.length(), 8);
    data.WriteBit(l_Guid1[0]); 
    data.WriteBit(l_Guid1[2]); 
    data.WriteBit(l_Guid1[5]); 
    data.WriteBit(l_Guid1[1]); 
    data.WriteBit(l_Guid1[7]);
    data.WriteBits(questOfferRewardText.length(), 12);
    data.WriteBit(l_Guid1[3]); 
    data.FlushBits();

    data.WriteByteSeq(l_Guid1[5]);
    data.WriteByteSeq(l_Guid1[0]);
    data.WriteString(questTurnTextWindow);
    data.WriteByteSeq(l_Guid1[4]);
    data.WriteByteSeq(l_Guid1[1]);

    for (uint8 i = 0; i < emoteCount; ++i)
    {
        data << uint32(quest->OfferRewardEmote[i]);
        data << uint32(quest->OfferRewardEmoteDelay[i]);    // Delay Emote
    }

    data.WriteString(questTitle);
    data.WriteByteSeq(l_Guid1[6]);
    data.WriteString(questGiverTargetName);
    data.WriteString(questGiverTextWindow);
    data.WriteString(questOfferRewardText);
    data.WriteByteSeq(l_Guid1[2]);
    data.WriteByteSeq(l_Guid1[3]);
    data.WriteByteSeq(l_Guid1[7]);
    data.WriteString(questTurnTargetName);    

    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverRequestItems(Quest const* quest, uint64 npcGUID, bool canComplete, bool closeOnCancel) const
{
    // We can always call to RequestItems, but this packet only goes out if there are actually
    // items.  Otherwise, we'll skip straight to the OfferReward

    std::string questTitle = quest->GetTitle();
    std::string requestItemsText = quest->GetRequestItemsText();

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->Title, locale, questTitle);
            ObjectMgr::GetLocaleString(localeData->RequestItemsText, locale, requestItemsText);
        }
    }

    if (!quest->GetReqItemsCount() && canComplete)
    {
        SendQuestGiverOfferReward(quest, npcGUID, true);
        return;
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

    WorldPacket data(SMSG_QUESTGIVER_REQUEST_ITEMS, 50);    // guess size
    ObjectGuid l_Guid = npcGUID;
    
    data << uint32(0);
    data << uint32(canComplete ? quest->GetCompleteEmote() : quest->GetIncompleteEmote());
    data << uint32(quest->GetFlags());
    data << uint32(quest->GetQuestId());
    data << uint32(0);                      // giver entry
    data << uint32(0);
    data << uint32(0);
    data << uint32(canComplete ? 95 : 0);   // Toggle "next" button
    data << uint32(0);

    //data << uint32(quest->GetRewOrReqMoney() < 0 ? -quest->GetRewOrReqMoney() : 0);

    data.WriteBit(closeOnCancel);
    data.WriteBit(l_Guid[5]);
    data.WriteBit(l_Guid[1]);
    data.WriteBit(l_Guid[0]);
    data.WriteBits(requestItemsText.length(), 12);
    data.WriteBit(l_Guid[4]);
    data.WriteBits(quest->GetReqCurrencyCount(), 21);
    data.WriteBits(quest->GetReqItemsCount(), 20);
    data.WriteBit(l_Guid[3]);
    data.WriteBit(l_Guid[2]);
    data.WriteBit(l_Guid[6]);
    data.WriteBit(l_Guid[7]);
    data.WriteBits(questTitle.length(), 9);
    data.FlushBits();

    data.WriteByteSeq(l_Guid[6]);

    for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
    {
        if (!quest->RequiredItemId[i])
            continue;

        if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(quest->RequiredItemId[i]))
            data << uint32(itemTemplate->DisplayInfoID);
        else
            data << uint32(0);

        data << uint32(quest->RequiredItemCount[i]);
        data << uint32(quest->RequiredItemId[i]);
    }

    data.WriteString(questTitle);

    for (int i = 0; i < QUEST_REQUIRED_CURRENCY_COUNT; ++i)
    {
        if (!quest->RequiredCurrencyId[i])
            continue;

        data << uint32(quest->RequiredCurrencyCount[i]);
        data << uint32(quest->RequiredCurrencyId[i]);
    }

    data.WriteByteSeq(l_Guid[4]);
    data.WriteString(requestItemsText);
    data.WriteByteSeq(l_Guid[0]);
    data.WriteByteSeq(l_Guid[7]);
    data.WriteByteSeq(l_Guid[2]);
    data.WriteByteSeq(l_Guid[5]);
    data.WriteByteSeq(l_Guid[1]);
    data.WriteByteSeq(l_Guid[3]);

    _session->SendPacket(&data);
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), quest->GetQuestId());
}

void PlayerMenu::AddQuestLevelToTitle(std::string &title, int32 level)
{
    // Adds the quest level to the front of the quest title
    // example: [13] Westfall Stew

    std::stringstream questTitlePretty;
    questTitlePretty << "[" << level << "] " << title;
    title = questTitlePretty.str();
}
