/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "MovementPacketBuilder.h"
#include "MoveSpline.h"
#include "WorldPacket.h"
#include "Object.h"
#include "Player.h"

uint32 hasUnkSplineCounter = false;

namespace Movement
{
    inline void operator << (ByteBuffer& b, const Vector3& v)
    {
        b << v.x << v.y << v.z;
    }

    inline void operator >> (ByteBuffer& b, Vector3& v)
    {
        b >> v.x >> v.y >> v.z;
    }

    enum MonsterMoveType
    {
        MonsterMoveNormal       = 0,
        MonsterMoveStop         = 1,
        MonsterMoveFacingSpot   = 2,
        MonsterMoveFacingTarget = 3,
        MonsterMoveFacingAngle  = 4
    };

	MonsterMoveType SplineFlagsToMonsterMoveType(MoveSplineFlag p_Flags)
    {
        switch (p_Flags & MoveSplineFlag::Mask_Final_Facing)
        {
            case MoveSplineFlag::Final_Target:
                return MonsterMoveFacingTarget;
                break;

            case MoveSplineFlag::Final_Angle:
                return MonsterMoveFacingAngle;
                break;

            case MoveSplineFlag::Final_Point:
                return MonsterMoveFacingSpot;
                break;
        }

        return MonsterMoveNormal;
    }

    void PacketBuilder::WriteCommonMonsterMovePart(const MoveSpline& p_MoveSpline, ObjectGuid p_CreatureGuid, ObjectGuid p_TransportGuid, WorldPacket& p_Data)
    {
        //////////////////////////////////////////////////////////////////////////
        /// Hack for cyclic movement / add fake Enter_Cycle flag - needed for client-side cyclic movement (client will erase first spline vertex after first cycle done)
        //////////////////////////////////////////////////////////////////////////

        MoveSplineFlag l_SplineFlags    = p_MoveSpline.splineflags;
        l_SplineFlags.enter_cycle       = p_MoveSpline.isCyclic();

        //////////////////////////////////////////////////////////////////////////
        /// Compute path nodes and header position
        //////////////////////////////////////////////////////////////////////////

        uint32              l_PathCount         = 0;
        uint32              l_SplineSplineOff   = 0;
        Movement::Location  l_Position          = Movement::Location(0, 0, 0, 0);
        Movement::Location  l_Origin            = Movement::Location(0, 0, 0, 0);

        Movement::SplineBase::ControlArray l_Path;
        
        Player* l_Player = 0;


        if (IS_PLAYER_GUID(p_CreatureGuid) && (l_Player = sObjectAccessor->FindPlayer(p_CreatureGuid)) != 0 && l_Player->m_taxi.GetCurrentTaxiPath())
        {
            l_Position          = p_MoveSpline.ComputePosition();
            l_PathCount         = p_MoveSpline.spline.getPointCount() - 2;
            l_SplineSplineOff   = 1;

            l_Path.push_back(p_MoveSpline.spline.getPoint(1+l_PathCount));

            for (uint32 l_I = 1 ; l_I < l_PathCount ; ++l_I)
                l_Path.push_back(p_MoveSpline.spline.getPoint(1+l_I));
        }
        else
        {
            l_Position  = p_MoveSpline.ComputePosition();
            l_PathCount = p_MoveSpline.spline.getPointCount() - 3;

            l_Path.push_back(p_MoveSpline.spline.getPoint(1+l_PathCount));

            for (uint32 l_I = 1 ; l_I < l_PathCount ; ++l_I)
                l_Path.push_back(p_MoveSpline.spline.getPoint(1+l_I));
        }

        //////////////////////////////////////////////////////////////////////////
        /// Write header spline advanced infos
        //////////////////////////////////////////////////////////////////////////

        uint32          l_SplineFlagsRaw        = uint32(l_SplineFlags & uint32(~MoveSplineFlag::Mask_No_Monster_Move));
        uint32          l_SplineDuration        = p_MoveSpline.Duration();
        float           l_VerticalAcceleration  = p_MoveSpline.vertical_acceleration;
        MonsterMoveType l_MoveType              = SplineFlagsToMonsterMoveType(l_SplineFlags);

        bool byte8C = false;

        uint32 l_AnimationTime = l_SplineFlags.animation ? p_MoveSpline.effect_start_time : 0;
        uint32 l_ParabolicTime = l_SplineFlags.parabolic ? p_MoveSpline.effect_start_time : 0;
        uint8 l_AnimationState = l_SplineFlags.animation ? l_SplineFlags.getAnimationId() : 0;

        uint8 byte38 = 0;
        uint8 byte6C = 0;
        uint8 byte6D = 0;
        uint8 byte78 = 0;

        uint32 dword4C = 0;
        uint32 dword90 = 0;

        p_Data.WriteBit(byte8C);
        p_Data.WriteBit(p_CreatureGuid[5]);
        p_Data.WriteBit(!l_AnimationTime);
        p_Data.WriteBit(!l_SplineDuration);
        p_Data.WriteBit(p_CreatureGuid[4]);
        p_Data.WriteBit(p_CreatureGuid[3]);
        p_Data.WriteBit(!dword4C);
        p_Data.WriteBit(!byte78);
        p_Data.WriteBit(p_CreatureGuid[2]);
        p_Data.WriteBit(!l_SplineFlagsRaw);
        p_Data.WriteBit(p_CreatureGuid[0]);

        if (byte8C)
        {
            p_Data.WriteBits(0, 2);
            p_Data.WriteBits(dword90, 22);
        }

        p_Data.WriteBits(l_PathCount != 0 ? l_PathCount - 1 : 0, 22);

        p_Data.WriteBit(!byte6D);
        p_Data.WriteBit(p_CreatureGuid[7]);
        p_Data.WriteBit(0);

        p_Data.WriteBit(p_TransportGuid[5]);
        p_Data.WriteBit(p_TransportGuid[3]);
        p_Data.WriteBit(p_TransportGuid[4]);
        p_Data.WriteBit(p_TransportGuid[6]);
        p_Data.WriteBit(p_TransportGuid[2]);
        p_Data.WriteBit(p_TransportGuid[1]);
        p_Data.WriteBit(p_TransportGuid[7]);
        p_Data.WriteBit(p_TransportGuid[0]);

        p_Data.WriteBit(!l_AnimationState);
        p_Data.WriteBit(!l_ParabolicTime);
        p_Data.WriteBit(!l_VerticalAcceleration);

        p_Data.WriteBit(p_CreatureGuid[6]);

        p_Data.WriteBits(l_SplineSplineOff > l_PathCount ? 0 : l_PathCount - l_SplineSplineOff, 20);

        p_Data.WriteBit(p_CreatureGuid[1]);

        p_Data.WriteBit(!byte6C);
        p_Data.WriteBits(l_MoveType, 3);

        if (l_MoveType == MonsterMoveFacingTarget)
        {
            ObjectGuid facingGuid = p_MoveSpline.facing.target;

            p_Data.WriteBit(facingGuid[4]);
            p_Data.WriteBit(facingGuid[6]);
            p_Data.WriteBit(facingGuid[5]);
            p_Data.WriteBit(facingGuid[1]);
            p_Data.WriteBit(facingGuid[0]);
            p_Data.WriteBit(facingGuid[7]);
            p_Data.WriteBit(facingGuid[3]);
            p_Data.WriteBit(facingGuid[2]);
        }

        p_Data.WriteBit(byte38);

        p_Data.FlushBits();

        //////////////////////////////////////////////////////////////////////////
        /// Write all spline data
        //////////////////////////////////////////////////////////////////////////

        if (byte8C)
        {
            for (uint32 l_I = 0 ; l_I < dword90 ; ++l_I)
            {
                p_Data << uint16(0);
                p_Data << uint16(0);
            }

            p_Data << uint16(0);
            p_Data << uint16(0);
            p_Data << float(0);
            p_Data << float(0);
        }

        p_Data.WriteByteSeq(p_TransportGuid[0]);
        p_Data.WriteByteSeq(p_TransportGuid[1]);
        p_Data.WriteByteSeq(p_TransportGuid[2]);
        p_Data.WriteByteSeq(p_TransportGuid[7]);
        p_Data.WriteByteSeq(p_TransportGuid[3]);
        p_Data.WriteByteSeq(p_TransportGuid[4]);
        p_Data.WriteByteSeq(p_TransportGuid[6]);
        p_Data.WriteByteSeq(p_TransportGuid[5]);

        if (l_MoveType == MonsterMoveFacingTarget)
        {
            ObjectGuid facingGuid = p_MoveSpline.facing.target;

            p_Data.WriteByteSeq(facingGuid[2]);
            p_Data.WriteByteSeq(facingGuid[1]);
            p_Data.WriteByteSeq(facingGuid[7]);
            p_Data.WriteByteSeq(facingGuid[0]);
            p_Data.WriteByteSeq(facingGuid[5]);
            p_Data.WriteByteSeq(facingGuid[3]);
            p_Data.WriteByteSeq(facingGuid[4]);
            p_Data.WriteByteSeq(facingGuid[6]);
        }

        p_Data << float(l_Position.y);

        p_Data.WriteByteSeq(p_CreatureGuid[7]);

        if (l_AnimationTime)
            p_Data << uint32(l_AnimationTime);

        if (l_Path.size())
        {
            Vector3 l_MiddleOffset = ((p_MoveSpline.spline.getPoint(1) + p_MoveSpline.spline.getPoint(1+l_PathCount)) / 2.f);

            for (uint32 l_I = 1 ; l_I < l_Path.size(); l_I++)
            {
                Vector3 offset = l_MiddleOffset - l_Path[l_I];
                p_Data.appendPackXYZ(offset.x, offset.y, offset.z);
            }
        }

        p_Data << float(0);
        p_Data << float(l_Position.z);
        p_Data << float(0);

        if (l_MoveType == MonsterMoveFacingSpot)
        {
            p_Data << p_MoveSpline.facing.f.z;
            p_Data << p_MoveSpline.facing.f.x;
            p_Data << p_MoveSpline.facing.f.y;
        }

        for (uint32 l_I = l_SplineSplineOff ; l_I < l_Path.size() ; ++l_I)
            p_Data << l_Path[l_I].y << l_Path[l_I].z << l_Path[l_I].x;

        p_Data.WriteByteSeq(p_CreatureGuid[5]);
        p_Data << uint32(p_MoveSpline.GetId());

        if (l_SplineFlagsRaw)
            p_Data << l_SplineFlagsRaw;

        if (l_AnimationState)
            p_Data << uint8(l_AnimationState);

        p_Data.WriteByteSeq(p_CreatureGuid[0]);

        if (byte6D)
            p_Data << uint8(byte6D);

        if (l_ParabolicTime)
            p_Data << uint32(l_ParabolicTime);

        p_Data << float(l_Position.x);

        if (l_SplineDuration)
            p_Data << uint32(l_SplineDuration);

        p_Data.WriteByteSeq(p_CreatureGuid[4]);

        if (l_VerticalAcceleration)
            p_Data << l_VerticalAcceleration;

        if (byte78)
            p_Data << uint8(byte78);

        if (l_MoveType == MonsterMoveFacingAngle)
            p_Data << p_MoveSpline.facing.angle;

        if (byte6C)
            p_Data << uint8(byte6C);

        p_Data << float(0);

        if (dword4C)
            p_Data << uint32(dword4C);

        p_Data.WriteByteSeq(p_CreatureGuid[6]);
        p_Data.WriteByteSeq(p_CreatureGuid[2]);
        p_Data.WriteByteSeq(p_CreatureGuid[3]);
        p_Data.WriteByteSeq(p_CreatureGuid[1]);
    }

    void PacketBuilder::WriteStopMovement(Vector3 const& pos, ObjectGuid p_CreatureGuid, ObjectGuid p_TransportGuid, uint32 splineId, ByteBuffer& p_Data)
    {
        uint32 l_SplineFlags = 0;

        p_Data.WriteBit(0);
        p_Data.WriteBit(p_CreatureGuid[5]);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(p_CreatureGuid[4]);
        p_Data.WriteBit(p_CreatureGuid[3]);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(p_CreatureGuid[2]);
        p_Data.WriteBit(!1);
        p_Data.WriteBit(p_CreatureGuid[0]);
        p_Data.WriteBits(0, 22);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(p_CreatureGuid[7]);
        p_Data.WriteBit(0);
        p_Data.WriteBit(p_TransportGuid[5]);
        p_Data.WriteBit(p_TransportGuid[3]);
        p_Data.WriteBit(p_TransportGuid[4]);
        p_Data.WriteBit(p_TransportGuid[6]);
        p_Data.WriteBit(p_TransportGuid[2]);
        p_Data.WriteBit(p_TransportGuid[1]);
        p_Data.WriteBit(p_TransportGuid[7]);
        p_Data.WriteBit(p_TransportGuid[0]);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(!0);
        p_Data.WriteBit(p_CreatureGuid[6]);
        p_Data.WriteBits(0, 20);
        p_Data.WriteBit(p_CreatureGuid[1]);
        p_Data.WriteBit(!0);
        p_Data.WriteBits(MonsterMoveStop, 3);
        p_Data.WriteBit(0);
        p_Data.FlushBits();

        //////////////////////////////////////////////////////////////////////////
        /// Write all spline data
        //////////////////////////////////////////////////////////////////////////

        p_Data.WriteByteSeq(p_TransportGuid[0]);
        p_Data.WriteByteSeq(p_TransportGuid[1]);
        p_Data.WriteByteSeq(p_TransportGuid[2]);
        p_Data.WriteByteSeq(p_TransportGuid[7]);
        p_Data.WriteByteSeq(p_TransportGuid[3]);
        p_Data.WriteByteSeq(p_TransportGuid[4]);
        p_Data.WriteByteSeq(p_TransportGuid[6]);
        p_Data.WriteByteSeq(p_TransportGuid[5]);
        p_Data << float(pos.y);
        p_Data.WriteByteSeq(p_CreatureGuid[7]);
        p_Data << float(0);
        p_Data << float(pos.z);
        p_Data << float(0);
        p_Data.WriteByteSeq(p_CreatureGuid[5]);
        p_Data << uint32(splineId);
        p_Data << l_SplineFlags;
        p_Data.WriteByteSeq(p_CreatureGuid[0]);
        p_Data << float(pos.x);
        p_Data.WriteByteSeq(p_CreatureGuid[4]);
        p_Data << float(0);
        p_Data.WriteByteSeq(p_CreatureGuid[6]);
        p_Data.WriteByteSeq(p_CreatureGuid[2]);
        p_Data.WriteByteSeq(p_CreatureGuid[3]);
        p_Data.WriteByteSeq(p_CreatureGuid[1]);
    }

    void WriteLinearPath(const Spline<int32>& spline, ByteBuffer& data)
    {
       /* uint32 last_idx = spline.getPointCount() - 3;
        const Vector3 * real_path = &spline.getPoint(1);

        data << last_idx;
        data << real_path[last_idx];   // destination
        if (last_idx > 1)
        {
            Vector3 middle = (real_path[0] + real_path[last_idx]) / 2.f;
            Vector3 offset;
            // first and last points already appended
            for (uint32 i = 1; i < last_idx; ++i)
            {
                offset = middle - real_path[i];
                data.appendPackXYZ(offset.x, offset.y, offset.z);
            }
        }*/
    }

    void WriteCatmullRomPath(const Spline<int32>& spline, ByteBuffer& data)
    {
        //uint32 count = spline.getPointCount() - 3;
       // data << count;
       // data.append<Vector3>(&spline.getPoint(2), count);
    }

    void WriteCatmullRomCyclicPath(const Spline<int32>& spline, ByteBuffer& data)
    {
        //uint32 count = spline.getPointCount() - 3;
        //data << uint32(count + 1);
        //data << spline.getPoint(1); // fake point, client will erase it from the spline after first cycle done
        //data.append<Vector3>(&spline.getPoint(1), count);
    }

   void PacketBuilder::WriteMonsterMove(const MoveSpline& move_spline, ObjectGuid p_CreatureGuid, ObjectGuid p_TransportGuid, WorldPacket& data)
    {
        WriteCommonMonsterMovePart(move_spline, p_CreatureGuid, p_TransportGuid, data);
    }

    void PacketBuilder::WriteCreateBits(MoveSpline const& moveSpline, ByteBuffer& data)
    {
       if (!data.WriteBit(!moveSpline.Finalized()))
            return;

        bool hasUnkSpline = false;

        data.WriteBit(moveSpline.splineflags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation));

        data.WriteBits(uint8(moveSpline.spline.mode()), 2);

        data.WriteBits(moveSpline.splineflags.raw(), 25);
        data.WriteBit(hasUnkSpline);

        if (hasUnkSpline)
        {
            data.WriteBits(0, 21);  // count
            data.WriteBits(0, 2);   // unk
        }

        data.WriteBits(moveSpline.getPath().size(), 20);
        data.WriteBit((moveSpline.splineflags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration());
    }

    void PacketBuilder::WriteCreateData(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (!moveSpline.Finalized())
        {
            MoveSplineFlag const& splineFlags = moveSpline.splineflags;

            switch (moveSpline.splineflags & MoveSplineFlag::Mask_Final_Facing)
            {
                case MoveSplineFlag::Final_Target:
                    data << uint8(MonsterMoveFacingTarget);
                    break;
                case MoveSplineFlag::Final_Angle:
                    data << uint8(MonsterMoveFacingAngle);
                    break;
                case MoveSplineFlag::Final_Point:
                    data << uint8(MonsterMoveFacingSpot);
                    break;
                default:
                    data << uint8(0);
                    break;
            }

            data << float(1.f);                             // splineInfo.duration_mod; added in 3.1

            uint32 nodes = moveSpline.getPath().size();
            for (uint32 i = 0; i < nodes; ++i)
            {
                data << float(moveSpline.getPath()[i].z);
                data << float(moveSpline.getPath()[i].x);
                data << float(moveSpline.getPath()[i].y);
            }

            uint32 hasUnkSplineCounter = 0;
            for (uint32 i = 0; i < hasUnkSplineCounter; ++i)
            {
                data << float(0.0f);
                data << float(0.0f);
            }

            data << float(1.f);                             // splineInfo.duration_mod_next; added in 3.1

            if ((moveSpline.splineflags & MoveSplineFlag::Mask_Final_Facing) == MoveSplineFlag::Final_Point)
                data << moveSpline.facing.f.z << moveSpline.facing.f.y <<moveSpline.facing.f.x;

            if ((moveSpline.splineflags & MoveSplineFlag::Mask_Final_Facing) == MoveSplineFlag::Final_Angle)
                data << moveSpline.facing.angle;

            if ((splineFlags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration())
                data << moveSpline.vertical_acceleration;   // added in 3.1

            if (splineFlags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation))
                data << moveSpline.effect_start_time;       // added in 3.1

            data << moveSpline.timePassed();
            data << moveSpline.Duration();
        }

        if (!moveSpline.isCyclic())
        {
            Vector3 dest = moveSpline.FinalDestination();
            data << moveSpline.GetId();
            data << float(dest.z);
            data << float(dest.x);
            data << float(dest.y);
        }
        else
        {
            data << moveSpline.GetId();
            data << float(0.0);
            data << float(0.0);
            data << float(0.0);
            data << float(0.0);
        }
    }
}