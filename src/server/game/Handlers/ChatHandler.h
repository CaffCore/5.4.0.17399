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

#ifndef __CHATHANDLER_H
#define __CHATHANDLER_H

#include "Common.h"
#include "Config.h"
#include <ace/Singleton.h>
#include "WorldPacket.h"

class ChatBuilder
{
    /* Construction */
    friend class ACE_Singleton<ChatBuilder, ACE_Null_Mutex>;
    ChatBuilder();

    public:
        ~ChatBuilder();

        bool BuildChatPacket(	WorldPacket * p_Dest, ObjectGuid p_Sender, ObjectGuid p_Receiver, ObjectGuid p_GroupGuid, ObjectGuid p_GuildGuid, const std::string & p_What, const std::string & p_Name,
								const std::string & p_ChannelName, uint8 p_ChatTag, uint8 p_Language, uint8 p_MessageType, uint32 p_RealmID, const char * p_AddonPrefix = 0);
};
#define sChatBuilder ACE_Singleton<ChatBuilder, ACE_Null_Mutex>::instance()

#endif	//	__CHATHANDLER_H
