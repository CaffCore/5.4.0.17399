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

#include "Common.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Pet.h"
#include "Player.h"
#include "SocialMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

class Aura;

/* differeces from off:
    -you can uninvite yourself - is is useful
    -you can accept invitation even if leader went offline
*/
/* todo:
    -group_destroyed msg is sent but not shown
    -reduce xp gaining when in raid group
    -quest sharing has to be corrected
    -FIX sending PartyMemberStats
*/

void WorldSession::SendPartyResult(PartyOperation operation, const std::string& member, PartyResult res, uint32 val /* = 0 */)
{
    WorldPacket data(SMSG_PARTY_COMMAND_RESULT, 4 + member.size() + 1 + 4 + 4 + 8);
    data << uint32(operation);
    data << member;
    data << uint32(res);
    data << uint32(val);                                    // LFD cooldown related (used with ERR_PARTY_LFG_BOOT_COOLDOWN_S and ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S)
    data << uint64(0); // player who caused error (in some cases).

    SendPacket(&data);
}

void WorldSession::HandleGroupInviteOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_INVITE");

    ObjectGuid crossRealmGuid; // unused
    std::string memberName, realmName;

    recvData.read_skip<uint32>(); // Non-zero in cross realm invites
    recvData.read_skip<uint32>(); // Always 0
    recvData.read_skip<uint8>();

    crossRealmGuid[7] = recvData.ReadBit();
    crossRealmGuid[0] = recvData.ReadBit();
    crossRealmGuid[4] = recvData.ReadBit();
    crossRealmGuid[3] = recvData.ReadBit();
    crossRealmGuid[6] = recvData.ReadBit();
    uint8 realmLen = recvData.ReadBits(9);
    crossRealmGuid[2] = recvData.ReadBit();
    crossRealmGuid[5] = recvData.ReadBit();
    crossRealmGuid[1] = recvData.ReadBit();
    uint8 nameLen = recvData.ReadBits(9);

    recvData.ReadByteSeq(crossRealmGuid[0]);
    recvData.ReadByteSeq(crossRealmGuid[4]);
    recvData.ReadByteSeq(crossRealmGuid[5]);
    recvData.ReadByteSeq(crossRealmGuid[7]);
    recvData.ReadByteSeq(crossRealmGuid[1]);
    realmName = recvData.ReadString(realmLen); // unused
    recvData.ReadByteSeq(crossRealmGuid[3]);
    memberName = recvData.ReadString(nameLen);
    recvData.ReadByteSeq(crossRealmGuid[6]);
    recvData.ReadByteSeq(crossRealmGuid[2]);

	// attempt add selected player

    // cheating
    if (!normalizePlayerName(memberName))
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    Player* player = sObjectAccessor->FindPlayerByName(memberName);

    // no player
    if (!player)
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // restrict invite to GMs
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_GM_GROUP) && !GetPlayer()->isGameMaster() && player->isGameMaster())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // can't group with
    if (!GetPlayer()->isGameMaster() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && GetPlayer()->GetTeam() != player->GetTeam())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_PLAYER_WRONG_FACTION);
        return;
    }
    if (GetPlayer()->GetInstanceId() != 0 && player->GetInstanceId() != 0 && GetPlayer()->GetInstanceId() != player->GetInstanceId() && GetPlayer()->GetMapId() == player->GetMapId())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_TARGET_NOT_IN_INSTANCE_S);
        return;
    }
    // just ignore us
    if (player->GetInstanceId() != 0 && player->GetDungeonDifficulty() != GetPlayer()->GetDungeonDifficulty())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_IGNORING_YOU_S);
        return;
    }

    if (player->GetSocial()->HasIgnore(GetPlayer()->GetGUIDLow()))
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_IGNORING_YOU_S);
        return;
    }

    ObjectGuid invitedGuid = player->GetGUID();

    Group* group = GetPlayer()->GetGroup();
    if (group && group->isBGGroup())
        group = GetPlayer()->GetOriginalGroup();

    Group* group2 = player->GetGroup();
    if (group2 && group2->isBGGroup())
        group2 = player->GetOriginalGroup();
    // player already in another group or invited
    if (group2 || player->GetGroupInvite())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_ALREADY_IN_GROUP_S);

        if (group2)
        {
            // tell the player that they were invited but it failed as they were already in a group
            WorldPacket data(SMSG_GROUP_INVITE, 45);

            data.WriteBit(0); // Show realm name
            data.WriteBit(1); // Is Cross Realm Or is already in a group
            data.WriteBit(invitedGuid[4]);
            data.WriteBits(GetPlayer()->GetName().size(), 6); // Inviter name length
            data.WriteBit(invitedGuid[2]);
            data.WriteBit(0);
            data.WriteBit(0); // Show ui
            data.WriteBit(invitedGuid[3]);
            data.WriteBit(invitedGuid[7]);
            data.WriteBits(realmName.length(), 9); // Realm name
            data.WriteBit(invitedGuid[0]);
            data.WriteBit(invitedGuid[5]);
            data.WriteBits(0, 22); // Count 2
            data.WriteBit(invitedGuid[6]);
            data.WriteBit(invitedGuid[1]);
            data.FlushBits();

            data << uint32(0);
            data << uint64(getMSTime());
            data.WriteByteSeq(invitedGuid[3]);
            data.WriteByteSeq(invitedGuid[5]);
            data << uint32(0);
            data.WriteString(realmName);
            data.WriteString(GetPlayer()->GetName()); // inviter name
            data << uint32(645);
            data.WriteByteSeq(invitedGuid[7]);
            data.WriteByteSeq(invitedGuid[4]);
            data.WriteByteSeq(invitedGuid[1]);
            // for count2 { int32(0) }
            data.WriteByteSeq(invitedGuid[2]);
            data.WriteByteSeq(invitedGuid[6]);
            data.WriteByteSeq(invitedGuid[0]);

            player->GetSession()->SendPacket(&data);
        }

        return;
    }

    if (group)
    {
        // not have permissions for invite
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_NOT_LEADER);
            return;
        }
        // not have place
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }
    }

    // ok, but group not exist, start a new group
    // but don't create and save the group to the DB until
    // at least one person joins
    if (!group)
    {
        group = new Group;
        // new group: if can't add then delete
        if (!group->AddLeaderInvite(GetPlayer()))
        {
            delete group;
            return;
        }
        if (!group->AddInvite(player))
        {
            delete group;
            return;
        }
    }
    else
    {
        // already existed group: if can't add then just leave
        if (!group->AddInvite(player))
        {
            return;
        }
    }

    // ok, we do it
    WorldPacket data(SMSG_GROUP_INVITE, 45);

    data.WriteBit(0); // Show realm name
    data.WriteBit(0); // Is Cross Realm Or is already in a group
    data.WriteBit(invitedGuid[4]);
    data.WriteBits(GetPlayer()->GetName().size(), 6); // Inviter name length
    data.WriteBit(invitedGuid[2]);
    data.WriteBit(0);
    data.WriteBit(1); // Show ui
    data.WriteBit(invitedGuid[3]);
    data.WriteBit(invitedGuid[7]);
    data.WriteBits(realmName.length(), 9); // Realm name
    data.WriteBit(invitedGuid[0]);
    data.WriteBit(invitedGuid[5]);
    data.WriteBits(0, 22); // Count 2
    data.WriteBit(invitedGuid[6]);
    data.WriteBit(invitedGuid[1]);
    data.FlushBits();

    data << uint32(0);
    data << uint64(getMSTime());
    data.WriteByteSeq(invitedGuid[3]);
    data.WriteByteSeq(invitedGuid[5]);
    data << uint32(0);
    data.WriteString(realmName);
    data.WriteString(GetPlayer()->GetName()); // inviter name
    data << uint32(645);
    data.WriteByteSeq(invitedGuid[7]);
    data.WriteByteSeq(invitedGuid[4]);
    data.WriteByteSeq(invitedGuid[1]);
    // for count2 { int32(0) }
    data.WriteByteSeq(invitedGuid[2]);
    data.WriteByteSeq(invitedGuid[6]);
    data.WriteByteSeq(invitedGuid[0]);

    player->GetSession()->SendPacket(&data);

    SendPartyResult(PARTY_OP_INVITE, memberName, ERR_PARTY_RESULT_OK);
}

void WorldSession::HandleGroupInviteResponseOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_INVITE_RESPONSE");

    uint8 unk1, unk2;
    uint32 unk3 = 0;
    recvData >> unk1;
    bool accept = recvData.ReadBit();
    unk2 = recvData.ReadBit();
    
    // Never actually received?
    if (unk2)
        recvData >> unk3; // unk

    Group* group = GetPlayer()->GetGroupInvite();

    if (!group)
        return;

    if (accept)
    {
        // Remove player from invitees in any case
        group->RemoveInvite(GetPlayer());

        if (group->GetLeaderGUID() == GetPlayer()->GetGUID())
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "HandleGroupAcceptOpcode: player %s(%d) tried to accept an invite to his own group", GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow());
            return;
        }

        // Group is full
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }

        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // Forming a new group, create it
        if (!group->IsCreated())
        {
            // This can happen if the leader is zoning. To be removed once delayed actions for zoning are implemented
            if (!leader)
            {
                group->RemoveAllInvites();
                return;
            }

            // If we're about to create a group there really should be a leader present
            ASSERT(leader);
            group->RemoveInvite(leader);
            group->Create(leader);
            sGroupMgr->AddGroup(group);
        }

        // Everything is fine, do it, PLAYER'S GROUP IS SET IN ADDMEMBER!!!
        if (!group->AddMember(GetPlayer()))
            return;

        group->BroadcastGroupUpdate();
    }
    else
    {
        // Remember leader if online (group pointer will be invalid if group gets disbanded)
        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // uninvite, group can be deleted
        GetPlayer()->UninviteFromGroup();

        if (!leader || !leader->GetSession())
            return;

        // report
        WorldPacket data(SMSG_GROUP_DECLINE, GetPlayer()->GetName().size());
        data << GetPlayer()->GetName();
        leader->GetSession()->SendPacket(&data);
    }
}

void WorldSession::HandleGroupUninviteGuidOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_UNINVITE_GUID");

    ObjectGuid guid;
    uint8 unk;

    recvData >> unk;
    guid[5] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    uint8 reasonLen = recvData.ReadBits(8);
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    std::string reason = recvData.ReadString(reasonLen);

    //can't uninvite yourself
    if (guid == GetPlayer()->GetGUID())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleGroupUninviteGuidOpcode: leader %s(%d) tried to uninvite himself from the group.",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow());
        return;
    }

    PartyResult res = GetPlayer()->CanUninviteFromGroup();
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res);
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (grp->IsLeader(guid))
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", ERR_NOT_LEADER);
        return;
    }

    if (grp->IsMember(guid))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID(), reason.c_str());
        return;
    }

    if (Player* player = grp->GetInvited(guid))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, "", ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupUninviteOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_UNINVITE");

    std::string membername;
    recvData >> membername;

    // player not found
    if (!normalizePlayerName(membername))
        return;

    // can't uninvite yourself
    if (GetPlayer()->GetName() == membername)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleGroupUninviteOpcode: leader %s(%d) tried to uninvite himself from the group.",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow());
        return;
    }

    PartyResult res = GetPlayer()->CanUninviteFromGroup();
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res);
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (uint64 guid = grp->GetMemberGUID(membername))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID());
        return;
    }

    if (Player* player = grp->GetInvited(membername))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, membername, ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupSetLeaderOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_SET_LEADER");

    ObjectGuid guid;
    uint8 unk;

    recvData >> unk;
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[7]);

    Player* player = ObjectAccessor::FindPlayer((uint64)guid);
    Group* group = GetPlayer()->GetGroup();

    if (!group || !player)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) || player->GetGroup() != group)
        return;

    // Everything's fine, accepted.
    group->ChangeLeader((uint64)guid);
    group->SendUpdate();
}

void WorldSession::HandleGroupSetRolesOpcode(WorldPacket& recvData)
{
	return;
 /* sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_SET_ROLES");

    uint8 unk;
    uint32 newRole;
    ObjectGuid SettedGuid;               // Target GUID    
    ObjectGuid Setter;                   // Assigner GUID

    Setter = GetPlayer()->GetGUID();

    recvData >> unk;
    recvData >> newRole;

    SettedGuid[7] = recvData.ReadBit();
    SettedGuid[4] = recvData.ReadBit();
    SettedGuid[0] = recvData.ReadBit();
    SettedGuid[2] = recvData.ReadBit();
    SettedGuid[6] = recvData.ReadBit();
    SettedGuid[5] = recvData.ReadBit();
    SettedGuid[1] = recvData.ReadBit();
    SettedGuid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(SettedGuid[0]);
    recvData.ReadByteSeq(SettedGuid[6]);
    recvData.ReadByteSeq(SettedGuid[3]);
    recvData.ReadByteSeq(SettedGuid[7]);
    recvData.ReadByteSeq(SettedGuid[1]);
    recvData.ReadByteSeq(SettedGuid[5]);
    recvData.ReadByteSeq(SettedGuid[4]);
    recvData.ReadByteSeq(SettedGuid[2]);

    sLog->outDebug(LOG_FILTER_NETWORKIO, "CMSG_GROUP_SET_ROLES unk %u", unk);

    
    if(Group* grp = GetPlayer()->GetGroup())
    {
        uint32 oldRole = grp->GetLfgRoles(SettedGuid);
        grp->SetLfgRoles(SettedGuid, newRole);
        WorldPacket data(SMSG_GROUP_SET_ROLE, 24);

        data << oldRole;
        data << uint8(0); //unk
        data << newRole;

        data.WriteBit(SettedGuid[0]);
        data.WriteBit(SettedGuid[3]);
        data.WriteBit(Setter[1]);
        data.WriteBit(Setter[7]);
        data.WriteBit(SettedGuid[5]);
        data.WriteBit(Setter[4]);
        data.WriteBit(Setter[3]);
        data.WriteBit(SettedGuid[2]);
        data.WriteBit(SettedGuid[7]);
        data.WriteBit(SettedGuid[6]);
        data.WriteBit(Setter[6]);
        data.WriteBit(SettedGuid[4]);
        data.WriteBit(Setter[0]);
        data.WriteBit(SettedGuid[1]);
        data.WriteBit(Setter[5]);
        data.WriteBit(Setter[2]);

        data.WriteByteSeq(Setter[3]);
        data.WriteByteSeq(SettedGuid[2]);
        data.WriteByteSeq(SettedGuid[6]);
        data.WriteByteSeq(Setter[1]);
        data.WriteByteSeq(SettedGuid[4]);
        data.WriteByteSeq(Setter[0]);
        data.WriteByteSeq(SettedGuid[1]);
        data.WriteByteSeq(Setter[6]);
        data.WriteByteSeq(Setter[2]);
        data.WriteByteSeq(SettedGuid[7]);
        data.WriteByteSeq(Setter[5]);
        data.WriteByteSeq(SettedGuid[3]);
        data.WriteByteSeq(Setter[4]);
        data.WriteByteSeq(Setter[7]);
        data.WriteByteSeq(SettedGuid[0]);
        data.WriteByteSeq(SettedGuid[5]);

        grp->BroadcastPacket(&data, false);
   }*/
}

void WorldSession::HandleGroupDisbandOpcode(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_DISBAND");

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (_player->InBattleground())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
        return;
    }

    /** error handling **/
    /********************/

    // everything's fine, do it
    SendPartyResult(PARTY_OP_LEAVE, GetPlayer()->GetName(), ERR_PARTY_RESULT_OK);

    GetPlayer()->RemoveFromGroup(GROUP_REMOVEMETHOD_LEAVE);
}

void WorldSession::HandleLootMethodOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_LOOT_METHOD");

    uint8 lootMethod, unk;
    uint32 lootThreshold;
    ObjectGuid lootMaster;

    recvData >> lootMethod >> unk >> lootThreshold;
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Received: CMSG_GROUP_SET_LOOT_PARAMS: lootMethod %u, lootThreshold %u, unk %u", lootMethod, lootThreshold, unk);

    lootMaster[6] = recvData.ReadBit();
    lootMaster[4] = recvData.ReadBit();
    lootMaster[7] = recvData.ReadBit();
    lootMaster[2] = recvData.ReadBit();
    lootMaster[5] = recvData.ReadBit();
    lootMaster[0] = recvData.ReadBit();
    lootMaster[1] = recvData.ReadBit();
    lootMaster[3] = recvData.ReadBit();

    recvData.ReadByteSeq(lootMaster[4]);
    recvData.ReadByteSeq(lootMaster[3]);
    recvData.ReadByteSeq(lootMaster[0]);
    recvData.ReadByteSeq(lootMaster[7]);
    recvData.ReadByteSeq(lootMaster[6]);
    recvData.ReadByteSeq(lootMaster[2]);
    recvData.ReadByteSeq(lootMaster[1]);
    recvData.ReadByteSeq(lootMaster[5]);

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    /** error handling **/
    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;
    /********************/

    // everything's fine, do it
    group->SetLootMethod((LootMethod)lootMethod);
    group->SetLooterGuid(lootMaster);
    group->SetLootThreshold((ItemQualities)lootThreshold);
    group->SendUpdate();
}

void WorldSession::HandleLootRoll(WorldPacket& recvData)
{
    uint8 itemSlot, rollType;
    ObjectGuid guid;

    recvData >> itemSlot;
    recvData >> rollType;              // 0: pass, 1: need, 2: greed

    sLog->outDebug(LOG_FILTER_NETWORKIO, "NOBODIE lootRoll %u %u", itemSlot, rollType);

    guid[3] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[7]);

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    group->CountRollVote(GetPlayer()->GetGUID(), guid, rollType);

    switch (rollType)
    {
        case ROLL_NEED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
            break;
        case ROLL_GREED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
    }
}

void WorldSession::HandleMinimapPingOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received MSG_MINIMAP_PING");

    if (!GetPlayer()->GetGroup())
        return;

    float x, y;
    recvData >> y;
    recvData >> x;
    recvData.read_skip<uint8>();

    WorldPacket data(SMSG_MINIMAP_PONG, (8+4+4));
    ObjectGuid l_Guid = GetPlayer()->GetGUID();

    data.WriteBit(l_Guid[6]);
    data.WriteBit(l_Guid[5]);
    data.WriteBit(l_Guid[1]);
    data.WriteBit(l_Guid[2]);
    data.WriteBit(l_Guid[4]);
    data.WriteBit(l_Guid[0]);
    data.WriteBit(l_Guid[3]);
    data.WriteBit(l_Guid[7]);

    data.WriteByteSeq(l_Guid[0]);
    data.WriteByteSeq(l_Guid[5]);
    data.WriteByteSeq(l_Guid[2]);
    data << float(x);
    data.WriteByteSeq(l_Guid[4]);
    data.WriteByteSeq(l_Guid[1]);
    data.WriteByteSeq(l_Guid[7]);
    data.WriteByteSeq(l_Guid[3]);
    data << float(y);
    data.WriteByteSeq(l_Guid[6]);

    GetPlayer()->GetGroup()->BroadcastPacket(&data, true, -1, GetPlayer()->GetGUID());
}

void WorldSession::HandleRandomRollOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received MSG_RANDOM_ROLL");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint8 x, unk;
    recvData >> unk >> x;



   if (x == 0xFF)                                           // target icon request
        group->SendTargetIconList(this);
    else                                                    // target icon update
    {
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;

        ObjectGuid guid;
        guid[2] = recvData.ReadBit();
        guid[1] = recvData.ReadBit();
        guid[6] = recvData.ReadBit();
        guid[4] = recvData.ReadBit();
        guid[5] = recvData.ReadBit();
        guid[0] = recvData.ReadBit();
        guid[7] = recvData.ReadBit();
        guid[3] = recvData.ReadBit();

        recvData.ReadByteSeq(guid[5]);
        recvData.ReadByteSeq(guid[4]);
        recvData.ReadByteSeq(guid[6]);
        recvData.ReadByteSeq(guid[0]);
        recvData.ReadByteSeq(guid[1]);
        recvData.ReadByteSeq(guid[2]);
        recvData.ReadByteSeq(guid[3]);
        recvData.ReadByteSeq(guid[7]);

        group->SetTargetIcon(x, _player->GetGUID(), guid);
   }
}

void WorldSession::HandleRaidTargetUpdateOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received MSG_RAID_TARGET_UPDATE");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint8 x, unk;
    recvData >> unk >> x;
    
    sLog->outDebug(LOG_FILTER_NETWORKIO, "RAID TARGET OPCODE (X: %u Y: %u)", x, unk);

    /** error handling **/
    /********************/

    // everything's fine, do it
    if (x == 0xFF)                                           // target icon request
        group->SendTargetIconList(this);
    else                                                    // target icon update
    {
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;

        ObjectGuid guid;
        guid[2] = recvData.ReadBit();
        guid[1] = recvData.ReadBit();
        guid[6] = recvData.ReadBit();
        guid[4] = recvData.ReadBit();
        guid[5] = recvData.ReadBit();
        guid[0] = recvData.ReadBit();
        guid[7] = recvData.ReadBit();
        guid[3] = recvData.ReadBit();

        recvData.ReadByteSeq(guid[5]);
        recvData.ReadByteSeq(guid[4]);
        recvData.ReadByteSeq(guid[6]);
        recvData.ReadByteSeq(guid[0]);
        recvData.ReadByteSeq(guid[1]);
        recvData.ReadByteSeq(guid[2]);
        recvData.ReadByteSeq(guid[3]);
        recvData.ReadByteSeq(guid[7]);

        group->SetTargetIcon(x, _player->GetGUID(), guid); // smsg part , need to check it too !
    }
}

void WorldSession::HandleGroupRaidConvertOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_RAID_CONVERT");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (_player->InBattleground())
        return;

    // error handling
    if (!group->IsLeader(GetPlayer()->GetGUID()) || group->GetMembersCount() < 2)
        return;

    // everything's fine, do it (is it 0 (PARTY_OP_INVITE) correct code)
    SendPartyResult(PARTY_OP_INVITE, "", ERR_PARTY_RESULT_OK);

    // New 4.x: it is now possible to convert a raid to a group if member count is 5 or less

    uint8 toRaid = recvData.ReadBit();
    
    if (toRaid)
        group->ConvertToRaid();
    else
        group->ConvertToGroup();
}

void WorldSession::HandleGroupChangeSubGroupOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_CHANGE_SUB_GROUP");

    // we will get correct pointer for group here, so we don't have to check if group is BG raid
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    std::string name;
    uint8 groupNr;
    recvData >> name;
    recvData >> groupNr;

    if (groupNr >= MAX_RAID_SUBGROUPS)
        return;

    uint64 senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    if (!group->HasFreeSlotSubGroup(groupNr))
        return;

    Player* movedPlayer = sObjectAccessor->FindPlayerByName(name);
    uint64 guid;

    if (movedPlayer)
        guid = movedPlayer->GetGUID();
    else
    {
        CharacterDatabase.EscapeString(name);
        guid = sObjectMgr->GetPlayerGUIDByName(name.c_str());
    }

    group->ChangeMembersGroup(guid, groupNr);
}

void WorldSession::HandleGroupSwapSubGroupOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_SWAP_SUB_GROUP");
    std::string unk1;
    std::string unk2;

    recvData >> unk1;
    recvData >> unk2;
}

void WorldSession::HandleGroupAssistantLeaderOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_ASSISTANT_LEADER");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    uint64 guid;
    bool apply;
    recvData >> guid;
    recvData >> apply;

    group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_ASSISTANT);

    group->SendUpdate();
}

void WorldSession::HandlePartyAssignmentOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received MSG_PARTY_ASSIGNMENT");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint64 senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    uint8 assignment;
    bool apply;
    uint64 guid;
    recvData >> assignment >> apply;
    recvData >> guid;

    switch (assignment)
    {
        case GROUP_ASSIGN_MAINASSIST:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINASSIST);
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINASSIST);
            break;
        case GROUP_ASSIGN_MAINTANK:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINTANK);           // Remove main assist flag from current if any.
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINTANK);
        default:
            break;
    }

    group->SendUpdate();
}

void WorldSession::HandleRaidReadyCheckOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received MSG_RAID_READY_CHECK");
	
	Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (recvData.empty())                                   // request
    {      

    /** error handling **/
    if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        return;
    /********************/

    uint8 unk;
    recvData >> unk;

    WorldPacket data(SMSG_RAID_READY_CHECK_STARTED, 8);
    ObjectGuid guid1 = GetPlayer()->GetGUID(), guid2 = 0;
    uint32 unk2 = 0;

    data.WriteBit(guid2[5]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid2[2]);
    data.WriteBit(guid1[1]);
    data.WriteBit(guid1[3]);
    data.WriteBit(guid1[2]);
    data.WriteBit(guid2[4]);
    data.WriteBit(guid2[0]);
    data.WriteBit(guid2[1]);
    data.WriteBit(guid1[5]);
    data.WriteBit(guid1[4]);
    data.WriteBit(guid1[0]);
    data.WriteBit(guid1[7]);
    data.WriteBit(guid2[6]);
    data.WriteBit(guid1[6]);
    data.WriteBit(guid2[7]);

    data.WriteByteSeq(guid1[7]);
    data.WriteByteSeq(guid2[7]);
    data.WriteByteSeq(guid1[3]);
    data.WriteByteSeq(guid2[2]);
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid1[5]);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid2[6]);
    data.WriteByteSeq(guid1[2]);
    data.WriteByteSeq(guid2[0]);
    data.WriteByteSeq(guid2[3]);
    data << unk;
    data.WriteByteSeq(guid1[0]);
    data.WriteByteSeq(guid1[4]);
    data.WriteByteSeq(guid2[4]);
    data.WriteByteSeq(guid1[1]);
    data.WriteByteSeq(guid1[6]);
    data << unk2;

    group->BroadcastPacket(&data, false, -1);

    group->OfflineReadyCheck();
	}
}

void WorldSession::HandleRaidReadyCheckFinishedOpcode(WorldPacket& /*recvData*/)
{
    //Group* group = GetPlayer()->GetGroup();
    //if (!group)
    //    return;

    //if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
    //    return;

    // Is any reaction need?
}

void WorldSession::BuildPartyMemberStatsChangedPacket(Player* player, WorldPacket* data)
{
    uint32 mask = player->GetGroupUpdateFlag();

    if (mask == GROUP_UPDATE_FLAG_NONE)
        return;

    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)                // if update power type, update current/max power also
        mask |= (GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER);

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)            // same for pets
        mask |= (GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER);

    data->Initialize(SMSG_PARTY_MEMBER_STATS, 80);          // average value
    data->append(player->GetPackGUID());
    *data << uint32(mask);

    if (mask & GROUP_UPDATE_FLAG_STATUS)
    {
        if (player)
        {
            if (player->IsPvP())
                *data << uint16(MEMBER_STATUS_ONLINE | MEMBER_STATUS_PVP);
            else
                *data << uint16(MEMBER_STATUS_ONLINE);
        }
        else
            *data << uint16(MEMBER_STATUS_OFFLINE);
    }

    if(mask & GROUP_UPDATE_FLAG_UNK2)
        *data << uint8(0) << uint8(0);

    if (mask & GROUP_UPDATE_FLAG_CUR_HP)
        *data << uint32(player->GetHealth());

    if (mask & GROUP_UPDATE_FLAG_MAX_HP)
        *data << uint32(player->GetMaxHealth());

    Powers powerType = player->getPowerType();
    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)
        *data << uint8(powerType);

    if(mask & GROUP_UPDATE_FLAG_POWER_TYPE2)
        *data << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_CUR_POWER)
        *data << uint16(player->GetPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_MAX_POWER)
        *data << uint16(player->GetMaxPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_LEVEL)
        *data << uint16(player->getLevel());

    if (mask & GROUP_UPDATE_FLAG_ZONE)
        *data << uint16(player->GetZoneId());

    if (mask & GROUP_UPDATE_FLAG_UNK100)
        *data << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_POSITION) {
        *data << uint16(player->GetPositionX());
        *data << uint16(player->GetPositionY());
        *data << uint16(player->GetPositionZ());
    }

    if (mask & GROUP_UPDATE_FLAG_AURAS)
    {
        *data << uint8(0);
        uint64 auramask = player->GetAuraUpdateMaskForRaid();
        *data << uint64(auramask);
        *data << uint32(MAX_AURAS); // count
        for (uint32 i = 0; i < MAX_AURAS; ++i)
        {
            if (auramask & (uint64(1) << i))
            {
                AuraApplication const* aurApp = player->GetVisibleAura(i);
                if (!aurApp)
                {
                    *data << uint32(0);
                    *data << uint8(0);
                    *data << uint32(0);
                    continue;
                }

                *data << uint32(aurApp->GetBase()->GetId());
                *data << uint8(aurApp->GetFlags());
                *data << uint32(0);

                if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                {
                    *data << uint8(MAX_SPELL_EFFECTS);
                    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    {
                        if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(i))
                            *data << float(eff->GetAmount());
                        else
                            *data << float(0);
                    }
                }
            }
        }
    }

    Pet* pet = player->GetPet();
    if (mask & GROUP_UPDATE_FLAG_PET_GUID)
    {
        if (pet)
            *data << uint64(pet->GetGUID());
        else
            *data << uint64(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_NAME)
    {
        if (pet)
            *data << pet->GetName();
        else
            *data << uint8(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MODEL_ID)
    {
        if (pet)
            *data << uint16(pet->GetDisplayId());
        else
            *data << uint16(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_HP)
    {
        if (pet)
            *data << uint32(pet->GetHealth());
        else
            *data << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_HP)
    {
        if (pet)
            *data << uint32(pet->GetMaxHealth());
        else
            *data << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
    {
        if (pet)
            *data << uint8(pet->getPowerType());
        else
            *data << uint8(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_POWER)
    {
        if (pet)
            *data << uint16(pet->GetPower(pet->getPowerType()));
        else
            *data << uint16(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_POWER)
    {
        if (pet)
            *data << uint16(pet->GetMaxPower(pet->getPowerType()));
        else
            *data << uint16(0);
    }    

    if (mask & GROUP_UPDATE_FLAG_PET_AURAS)
    {
        if (pet)
        {
            *data << uint8(0);
            uint64 auramask = pet->GetAuraUpdateMaskForRaid();
            *data << uint64(auramask);
            *data << uint32(MAX_AURAS); // count
            for (uint32 i = 0; i < MAX_AURAS; ++i)
            {
                if (auramask & (uint64(1) << i))
                {
                    AuraApplication const* aurApp = pet->GetVisibleAura(i);
                    if (!aurApp)
                    {
                        *data << uint32(0);
                        *data << uint8(0);
                        *data << uint32(0);
                        continue;
                    }

                    *data << uint32(aurApp->GetBase()->GetId());
                    *data << uint8(aurApp->GetFlags());
                    *data << uint32(0);

                    if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                    {
                        *data << uint8(MAX_SPELL_EFFECTS);

                        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        {
                            if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(i))
                                *data << float(eff->GetAmount());
                            else
                                *data << float(0);
                        }
                    }
                }
            }
        }
        else
        {
            *data << uint8(0);
            *data << uint64(0);
        }
    }

    if (mask & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
    {
        if (Vehicle* veh = player->GetVehicle())
            *data << uint32(veh->GetVehicleInfo()->m_seatID[player->m_movementInfo.t_seat]);
        else
            *data << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PHASE)
    {
        *data << uint32(8); // either 0 or 8, same unk found in SMSG_PHASESHIFT
        data->WriteBits(0, 25);
        data->FlushBits();
        // for (count) *data << uint16(phaseId)
    }
}

/*this procedure handles clients CMSG_REQUEST_PARTY_MEMBER_STATS request*/
void WorldSession::HandleRequestPartyMemberStatsOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint8 unk;

    recvData >> unk;

    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);

    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_REQUEST_PARTY_MEMBER_STATS %u %u", unk, GUID_LOPART(guid));

    Player* player = HashMapHolder<Player>::Find(guid);
    if (!player)
    {
        WorldPacket data(SMSG_PARTY_MEMBER_STATS_FULL, 3+4+2);
        data << uint8(0);                                   // only for SMSG_PARTY_MEMBER_STATS_FULL, probably arena/bg related
        data.appendPackGUID(guid);
        data << (uint32) GROUP_UPDATE_FLAG_STATUS;
        data << (uint16) MEMBER_STATUS_OFFLINE;
        SendPacket(&data);
        return;
    }

    Pet* pet = player->GetPet();

    WorldPacket data(SMSG_PARTY_MEMBER_STATS_FULL, 4+2+2+2+1+2*6+8+1+8);
    data << uint8(0);                                       // only for SMSG_PARTY_MEMBER_STATS_FULL, probably arena/bg related
    data.append(player->GetPackGUID());

    uint32 mask1 = GROUP_UPDATE_FULL;

    if (!pet)
        mask1 &= ~GROUP_UPDATE_PET;

    //Powers powerType = player->getPowerType();
    data << uint32(mask1);                                // group update mask
    data << uint16(MEMBER_STATUS_ONLINE);                 // member's online status, GROUP_UPDATE_FLAG_STATUS

    if(mask1 & GROUP_UPDATE_FLAG_UNK2)
        data << uint8(0) << uint8(0);

    if (mask1 & GROUP_UPDATE_FLAG_CUR_HP)
        data << uint32(player->GetHealth());

    if (mask1 & GROUP_UPDATE_FLAG_MAX_HP)
        data << uint32(player->GetMaxHealth());

    Powers powerType = player->getPowerType();
    if (mask1 & GROUP_UPDATE_FLAG_POWER_TYPE)
        data << uint8(powerType);

    if(mask1 & GROUP_UPDATE_FLAG_POWER_TYPE2)
        data << uint16(0);

    if (mask1 & GROUP_UPDATE_FLAG_CUR_POWER)
        data << uint16(player->GetPower(powerType));

    if (mask1 & GROUP_UPDATE_FLAG_MAX_POWER)
        data << uint16(player->GetMaxPower(powerType));

    if (mask1 & GROUP_UPDATE_FLAG_LEVEL)
        data << uint16(player->getLevel());

    if (mask1 & GROUP_UPDATE_FLAG_ZONE)
        data << uint16(player->GetZoneId());

    if (mask1 & GROUP_UPDATE_FLAG_UNK100)
        data << uint16(0);

    if (mask1 & GROUP_UPDATE_FLAG_POSITION) {
        data << uint16(player->GetPositionX());
        data << uint16(player->GetPositionY());
        data << uint16(player->GetPositionZ());
    }

    if (mask1 & GROUP_UPDATE_FLAG_AURAS)
    {
        data << uint8(0);
        uint64 auramask1 = player->GetAuraUpdateMaskForRaid();
        data << uint64(auramask1);
        data << uint32(MAX_AURAS); // count
        for (uint32 i = 0; i < MAX_AURAS; ++i)
        {
            if (auramask1 & (uint64(1) << i))
            {
                AuraApplication const* aurApp = player->GetVisibleAura(i);
                if (!aurApp)
                {
                    data << uint32(0);
                    data << uint8(0);
                    data << uint32(0);
                    continue;
                }

                data << uint32(aurApp->GetBase()->GetId());
                data << uint8(aurApp->GetFlags());
                data << uint32(0);

                if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                {
                    data << uint8(MAX_SPELL_EFFECTS);
                    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    {
                        if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(i))
                            data << float(eff->GetAmount());
                        else
                            data << float(0);
                    }
                }
            }
        }
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_GUID)
    {
        if (pet)
            data << uint64(pet->GetGUID());
        else
            data << uint64(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_NAME)
    {
        if (pet)
            data << pet->GetName();
        else
            data << uint8(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_MODEL_ID)
    {
        if (pet)
            data << uint16(pet->GetDisplayId());
        else
            data << uint16(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_CUR_HP)
    {
        if (pet)
            data << uint32(pet->GetHealth());
        else
            data << uint32(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_MAX_HP)
    {
        if (pet)
            data << uint32(pet->GetMaxHealth());
        else
            data << uint32(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
    {
        if (pet)
            data << uint8(pet->getPowerType());
        else
            data << uint8(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_CUR_POWER)
    {
        if (pet)
            data << uint16(pet->GetPower(pet->getPowerType()));
        else
            data << uint16(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_MAX_POWER)
    {
        if (pet)
            data << uint16(pet->GetMaxPower(pet->getPowerType()));
        else
            data << uint16(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PET_AURAS)
    {
        if (pet)
        {
            data << uint8(0);
            uint64 auramask1 = pet->GetAuraUpdateMaskForRaid();
            data << uint64(auramask1);
            data << uint32(MAX_AURAS); // count
            for (uint32 i = 0; i < MAX_AURAS; ++i)
            {
                if (auramask1 & (uint64(1) << i))
                {
                    AuraApplication const* aurApp = pet->GetVisibleAura(i);
                    if (!aurApp)
                    {
                        data << uint32(0);
                        data << uint8(0);
                        data << uint32(0);
                        continue;
                    }

                    data << uint32(aurApp->GetBase()->GetId());
                    data << uint8(aurApp->GetFlags());
                    data << uint32(0);

                    if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                    {
                        data << uint8(MAX_SPELL_EFFECTS);

                        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        {
                            if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(i))
                                data << float(eff->GetAmount());
                            else
                                data << float(0);
                        }
                    }
                }
            }
        }
        else
        {
            data << uint8(0);
            data << uint64(0);
        }
    }

    if (mask1 & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
    {
        if (Vehicle* veh = player->GetVehicle())
            data << uint32(veh->GetVehicleInfo()->m_seatID[player->m_movementInfo.t_seat]);
        else
            data << uint32(0);
    }

    if (mask1 & GROUP_UPDATE_FLAG_PHASE)
    {
        data << uint32(8); // either 0 or 8, same unk found in SMSG_PHASESHIFT
        data.WriteBits(0, 25);
        data.FlushBits();
        // for (count) data << uint16(phaseId)
    }

    SendPacket(&data);
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket& /*recvData*/)
{
    // every time the player checks the character screen
    _player->SendRaidInfo();
}

void WorldSession::HandleOptOutOfLootOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_OPT_OUT_OF_LOOT");

    bool passOnLoot = recvData.ReadBit(); // 1 always pass, 0 do not pass

    // ignore if player not loaded
    if (!GetPlayer())                                        // needed because STATUS_AUTHED
    {
        if (passOnLoot)
            sLog->outError(LOG_FILTER_NETWORKIO, "CMSG_OPT_OUT_OF_LOOT value<>0 for not-loaded character!");
        return;
    }

    GetPlayer()->SetPassOnGroupLoot(passOnLoot);
}
