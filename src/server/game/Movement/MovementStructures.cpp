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

#include "MovementStructures.h"
#include "Player.h"

//5.4.0
MovementStatusElements const PlayerMoveSequence[] =
{
    MSEZeroBit,
    MSEHasPitch,
    MSEHasGuidByte4,
    MSEHasGuidByte2,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasGuidByte7,
    MSECounter,
    MSEHasGuidByte5,
    MSEHasGuidByte3,
    MSEHasATime,
    MSEHasTransportData,

    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,
    MSEHasTransportTime3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,

    MSEHasMovementFlags,
    MSEMovementFlags,
    MSEHasOrientation,
    MSEHasTimestamp,
    MSEHasFallDirection,
    MSEHasMovementFlags2,
    MSEHasGuidByte6,
    MSEHasGuidByte0,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEMovementFlags2,
    MSEHasSplineElevation,

    MSEPositionX,

    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEGuidByte3,

    MSETransportGuidByte2,
    MSETransportGuidByte0,
    MSETransportGuidByte5,
    MSETransportSeat,
    MSETransportGuidByte4,
    MSETransportGuidByte3,
    MSETransportTime2,
    MSETransportGuidByte6,
    MSETransportGuidByte7,
    MSETransportPositionX,
    MSETransportTime3,
    MSETransportTime,
    MSETransportPositionZ,
    MSETransportGuidByte1,
    MSETransportPositionY,
    MSETransportOrientation,

    MSEGuidByte2,
    MSEGuidByte6,

    //MSECounterUint32,

    MSEGuidByte1,
    MSEPitch,
    MSEPositionY,
    MSEPositionZ,
    MSEGuidByte4,
    MSETimestamp,
    MSESplineElevation,
    MSEATime,
    MSEGuidByte0,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementFallLandSequence[] =
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSECounter,
    MSEHasGuidByte6,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSEHasGuidByte3,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEHasMovementFlags2,
    MSEHasGuidByte1,
    MSEHasGuidByte2,
    MSEHasMovementFlags,
    MSEZeroBit,
    MSEHasTransportData,
    MSEHasGuidByte5,
    MSEHasOrientation,
    MSEHasTimestamp,
    MSEHasFallData,
    MSEHasATime,
    MSEHasPitch,
    MSEHasGuidByte4,
    MSEHasSplineElevation,

    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte5,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime3,

    MSEMovementFlags,
    MSEMovementFlags2,
    MSEHasFallDirection,

    MSEGuidByte7,
    MSEGuidByte1,
    MSEGuidByte0,
    MSEGuidByte3,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte6,
    MSEGuidByte5,
    MSEGuidByte2,

    MSEFallTime,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,

    MSETransportPositionZ,
    MSETransportGuidByte7,
    MSETransportPositionX,
    MSETransportGuidByte0,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportTime3,
    MSETransportGuidByte3,
    MSETransportGuidByte2,
    MSETransportGuidByte6,
    MSETransportPositionY,
    MSETransportTime,
    MSETransportGuidByte5,
    MSETransportGuidByte4,
    MSETransportOrientation,
    MSETransportSeat,

    MSETimestamp,
    MSEPitch,
    MSESplineElevation,
    MSEOrientation,
    MSEATime,

    MSEEnd,
};

//5.1.0 16357 OK
MovementStatusElements const MovementFallResetSequence[] =
{
    MSEPositionY,
    MSEPositionX,
    MSEPositionZ,
    MSEHasGuidByte2,
    MSEHasPitch,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEHasOrientation,
    //MSEIsAlive,
    MSEZeroBit,
    MSEHasFallData,
    //MSEHasCounter,
    MSEHasMovementFlags2,
    MSEHasSpline,
    MSEHasGuidByte7,
    MSEHasTransportData,
    MSEHasTimestamp,
    MSEHasGuidByte5,
    MSEHasSplineElevation,
    MSEHasGuidByte3,
    MSEHasMovementFlags,
    MSEHasGuidByte1,
    MSEHasGuidByte4,
    MSEHasGuidByte6,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte0,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasFallDirection,
    MSEMovementFlags2,
    MSEMovementFlags,
    MSEGuidByte1,
    MSEGuidByte0,
    MSEGuidByte7,
    MSECounter,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte2,
    MSEGuidByte3,
    MSEGuidByte4,
    MSETransportPositionX,
    MSETransportGuidByte0,
    MSETransportTime2,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportGuidByte3,
    MSETransportSeat,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportPositionZ,
    MSETransportPositionY,
    MSETransportGuidByte6,
    MSETransportTime,
    MSETransportGuidByte2,
    MSETransportTime3,
    MSETransportGuidByte1,
    MSEPitch,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallVerticalSpeed,
    MSEFallTime,
    MSEOrientation,
    MSETimestamp,
    MSESplineElevation,
    //MSEAlive,
    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementHeartBeatSequence[] =
{
   MSEPositionZ,
    MSEPositionX,
    MSEPositionY,
    MSEHasGuidByte1,
    MSEHasGuidByte3,
    MSEHasMovementFlags,
    MSEHasGuidByte0,
    MSEHasOrientation,
    MSEHasGuidByte4,
    MSEHasPitch,
    MSEHasGuidByte2,
    MSEZeroBit,
    MSEZeroBit,
    MSECounter,
    MSEHasFallData,
    MSEHasGuidByte5,
    MSEHasGuidByte7,
    MSEHasSplineElevation,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasATime,
    MSEHasTimestamp,
    MSEHasMovementFlags2,
    MSEHasGuidByte6,

    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte6,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,

    MSEMovementFlags,
    MSEMovementFlags2,
    MSEHasFallDirection,

    MSEGuidByte3,
    MSEGuidByte4,
    MSEGuidByte7,
    MSEGuidByte1,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEGuidByte5,
    MSECounterUint32,
    MSEGuidByte6,

    MSETransportGuidByte6,
    MSETransportGuidByte2,
    MSETransportPositionY,
    MSETransportGuidByte5,
    MSETransportTime,
    MSETransportPositionX,
    MSETransportGuidByte1,
    MSETransportGuidByte3,
    MSETransportTime3,
    MSETransportGuidByte4,
    MSETransportPositionZ,
    MSETransportGuidByte0,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportTime2,
    MSETransportSeat,

    MSESplineElevation,

    MSEFallTime,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,

    MSEPitch,
    MSETimestamp,
    MSEATime,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementJumpSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    MSEHasGuidByte0,
    MSEHasFallData,
    MSEHasATime,
    MSEZeroBit,
    MSEZeroBit,
    MSEHasMovementFlags,
    MSEHasPitch,
    MSEHasGuidByte5,
    MSEHasGuidByte1,
    MSEHasTimestamp,
    MSEHasGuidByte2,
    MSECounter,
    MSEHasGuidByte7,
    MSEHasSplineElevation,
    MSEHasGuidByte6,
    MSEHasTransportData,
    MSEHasMovementFlags2,
    MSEHasOrientation,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSEHasGuidByte3,

    MSEHasTransportTime3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte5,

    MSEHasFallDirection,
    MSEMovementFlags,
    MSEMovementFlags2,

    MSECounterUint32,

    MSEGuidByte0,
    MSEGuidByte3,
    MSEGuidByte6,
    MSEGuidByte1,
    MSEGuidByte7,
    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte5,

    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportOrientation,
    MSETransportTime2,
    MSETransportGuidByte6,
    MSETransportGuidByte3,
    MSETransportSeat,
    MSETransportPositionX,
    MSETransportGuidByte1,
    MSETransportTime3,
    MSETransportGuidByte2,
    MSETransportGuidByte7,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportPositionY,
    MSETransportGuidByte0,

    MSETimestamp,
    MSEATime,
    MSEPitch,
    MSEOrientation,
    MSESplineElevation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementSetFacingSequence[] =
{
	 MSEPositionY,
    MSEPositionX,
    MSEPositionZ,
    MSEHasMovementFlags,
    MSEHasGuidByte1,
    MSEHasOrientation,
    MSEZeroBit,
    MSEHasGuidByte4,
    MSEHasGuidByte3,
    MSEHasMovementFlags2,
    MSEHasPitch,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSECounter,
    MSEHasGuidByte5,
    MSEHasATime,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasGuidByte2,
    MSEHasSplineElevation,
    MSEHasTimestamp,
    MSEHasGuidByte0,
    MSEHasFallData,

    MSEMovementFlags,
    MSEMovementFlags2,

    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte0,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte6,

    MSEHasFallDirection,

    MSEGuidByte1,
    MSEGuidByte2,
    MSEGuidByte6,
    MSECounterUint32,
    MSEGuidByte3,
    MSEGuidByte4,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte5,

    MSETransportPositionZ,
    MSETransportGuidByte2,
    MSETransportTime3,
    MSETransportGuidByte6,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportGuidByte5,
    MSETransportSeat,
    MSETransportGuidByte3,
    MSETransportGuidByte0,
    MSETransportPositionY,
    MSETransportPositionX,
    MSETransportGuidByte4,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportTime,

    MSEATime,

    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallVerticalSpeed,
    MSEFallTime,

    MSESplineElevation,
    MSETimestamp,
    MSEPitch,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementSetPitchSequence[] =
{
    MSEPositionY,
    MSEPositionZ,
    MSEPositionX,
    MSEHasGuidByte4,
    MSEHasPitch,
    MSEHasTimestamp,
    MSEHasGuidByte3,
    MSEHasTransportData,
    MSEHasATime,
    MSECounter,
    MSEHasGuidByte1,
    MSEHasOrientation,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasFallData,
    MSEHasMovementFlags2,
    MSEHasGuidByte5,
    MSEZeroBit,
    MSEHasGuidByte2,
    MSEHasSplineElevation,
    MSEHasGuidByte7,
    MSEHasMovementFlags,
    MSEZeroBit,

    MSEMovementFlags,

    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportTime3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte1,

    MSEHasFallDirection,
    MSEMovementFlags2,

    MSEGuidByte1,
    MSEGuidByte6,
    MSEGuidByte5,
    MSEGuidByte2,
    MSEGuidByte7,
    MSEGuidByte0,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte3,

    MSETransportPositionY,
    MSETransportGuidByte6,
    MSETransportGuidByte2,
    MSETransportGuidByte5,
    MSETransportGuidByte1,
    MSETransportGuidByte3,
    MSETransportPositionZ,
    MSETransportSeat,
    MSETransportOrientation,
    MSETransportTime3,
    MSETransportGuidByte0,
    MSETransportTime2,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportPositionX,
    MSETransportGuidByte7,

    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallVerticalSpeed,
    MSEFallTime,

    MSESplineElevation,
    MSETimestamp,
    MSEATime,
    MSEOrientation,
    MSEPitch,

    MSEEnd,
};

//5.4.0
MovementStatusElements MovementStartBackwardSequence[] =
{
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasOrientation,
    MSEHasPitch,
    MSECounter,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEHasMovementFlags,
    MSEZeroBit,
    MSEHasATime,
    MSEHasGuidByte5,
    MSEHasTimestamp,
    MSEHasSplineElevation,
    MSEHasGuidByte2,
    MSEHasGuidByte7,
    MSEHasGuidByte6,
    MSEHasGuidByte3,
    MSEHasTransportData,
    MSEHasGuidByte0,
    MSEHasGuidByte4,
    MSEHasMovementFlags2,

    MSEHasTransportTime3,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,

    MSEMovementFlags2,
    MSEMovementFlags,
    MSEHasFallDirection,

    MSEGuidByte3,
    MSEGuidByte6,
    MSEGuidByte0,
    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte7,
    MSECounterUint32,
    MSEGuidByte5,
    MSEGuidByte1,

    MSETransportTime,
    MSETransportTime3,
    MSETransportSeat,
    MSETransportGuidByte3,
    MSETransportPositionY,
    MSETransportGuidByte7,
    MSETransportPositionX,
    MSETransportTime2,
    MSETransportPositionZ,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportOrientation,
    MSETransportGuidByte1,
    MSETransportGuidByte6,
    MSETransportGuidByte2,
    MSETransportGuidByte0,

    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSESplineElevation,
    MSEPitch,
    MSETimestamp,
    MSEATime,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartForwardSequence[] =
{
    MSEPositionZ,
    MSEPositionX,
    MSEPositionY,
    MSEHasOrientation,
    MSEHasPitch,
    MSEHasGuidByte4,
    MSEHasGuidByte3,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasGuidByte2,
    MSEHasMovementFlags,
    MSEHasGuidByte0,
    MSECounter,
    MSEHasGuidByte6,
    MSEHasATime,
    MSEHasMovementFlags2,
    MSEHasTimestamp,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasGuidByte5,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSEHasGuidByte1,
    MSEHasSplineElevation,

    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte2,
    MSEHasTransportTime2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,

    MSEMovementFlags,
    MSEHasFallDirection,
    MSEMovementFlags2,

    MSEGuidByte6,
    MSEGuidByte4,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte5,
    MSEGuidByte2,
    MSECounterUint32,
    MSEGuidByte3,

    MSETransportGuidByte0,
    MSETransportGuidByte5,
    MSETransportGuidByte6,
    MSETransportGuidByte7,
    MSETransportTime3,
    MSETransportGuidByte2,
    MSETransportOrientation,
    MSETransportTime,
    MSETransportSeat,
    MSETransportGuidByte1,
    MSETransportPositionZ,
    MSETransportPositionX,
    MSETransportGuidByte4,
    MSETransportTime2,
    MSETransportGuidByte3,
    MSETransportPositionY,

    MSETimestamp,
    MSEPitch,

    MSEFallTime,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,

    MSEATime,
    MSEOrientation,
    MSESplineElevation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartStrafeLeftSequence[] =
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSEZeroBit,
    MSEHasGuidByte2,
    MSEHasGuidByte6,
    MSEHasGuidByte0,
    MSEHasGuidByte1,
    MSEHasGuidByte5,
    MSEHasTimestamp,
    MSEHasSplineElevation,
    MSEZeroBit,
    MSEHasGuidByte4,
    MSEHasTransportData,
    MSEHasPitch,
    MSEHasFallData,
    MSEHasMovementFlags,
    MSECounter,
    MSEHasGuidByte3,
    MSEHasOrientation,
    MSEHasMovementFlags2,
    MSEZeroBit,
    MSEHasATime,
    MSEHasGuidByte7,
    MSEHasFallDirection,

    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte6,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte5,

    MSEMovementFlags,
    MSEMovementFlags2,

    MSEGuidByte2,
    MSEGuidByte7,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte3,
    MSEGuidByte0,
    MSECounterUint32,
    MSEGuidByte1,

    MSEOrientation,

    MSETransportGuidByte1,
    MSETransportGuidByte0,
    MSETransportGuidByte6,
    MSETransportGuidByte2,
    MSETransportTime2,
    MSETransportPositionZ,
    MSETransportGuidByte3,
    MSETransportTime,
    MSETransportPositionY,
    MSETransportGuidByte4,
    MSETransportSeat,
    MSETransportTime3,
    MSETransportPositionX,
    MSETransportOrientation,
    MSETransportGuidByte7,
    MSETransportGuidByte5,

    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEATime,
    MSESplineElevation,
    MSETimestamp,
    MSEPitch,
	MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartStrafeRightSequence[] =
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSEHasGuidByte6,
    MSEHasGuidByte3,
    MSEHasMovementFlags2,
    MSEHasPitch,
    MSEHasOrientation,
    MSEHasSplineElevation,
    MSEZeroBit,
    MSEHasATime,
    MSEHasMovementFlags,
    MSEHasFallData,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSECounter,
    MSEHasGuidByte7,
    MSEHasGuidByte1,
    MSEHasGuidByte5,
    MSEHasTimestamp,
    MSEHasTransportData,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEHasGuidByte2,

    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte5,

    MSEHasFallDirection,

    MSEMovementFlags,
    MSEMovementFlags2,

    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte0,
    MSEGuidByte7,
    MSECounterUint32,
    MSEGuidByte6,
    MSEGuidByte5,
    MSEGuidByte3,
    MSEGuidByte1,

    MSETransportGuidByte4,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportTime2,
    MSETransportGuidByte0,
    MSETransportSeat,
    MSETransportTime3,
    MSETransportPositionZ,
    MSETransportPositionX,
    MSETransportGuidByte1,
    MSETransportTime,
    MSETransportGuidByte6,
    MSETransportPositionY,
    MSETransportGuidByte2,
    MSETransportGuidByte5,
    MSETransportGuidByte3,

    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSETimestamp,
    MSEPitch,
    MSEOrientation,
    MSESplineElevation,
    MSEATime,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartTurnLeftSequence[] =
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSEHasGuidByte4,
    MSEHasPitch,
    MSEHasGuidByte2,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEHasGuidByte5,
    MSEHasATime,
    MSEHasSplineElevation,
    MSEHasTimestamp,
    MSEHasGuidByte6,
    MSEHasTransportData,
    MSEHasGuidByte3,
    MSEHasOrientation,
    MSEZeroBit,
    MSEHasGuidByte0,
    MSEHasGuidByte7,
    MSEHasMovementFlags,
    MSEHasFallData,
    MSEZeroBit,
    MSECounter,
    MSEHasMovementFlags2,

    MSEHasTransportTime3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte4,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte1,

    MSEMovementFlags2,
    MSEMovementFlags,

    MSEHasFallDirection,

    MSEGuidByte7,
    MSECounterUint32,
    MSEGuidByte2,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte0,
    MSEGuidByte4,
    MSEGuidByte3,
    MSEGuidByte1,

    MSETransportPositionZ,
    MSETransportGuidByte0,
    MSETransportGuidByte3,
    MSETransportGuidByte1,
    MSETransportPositionX,
    MSETransportGuidByte2,
    MSETransportSeat,
    MSETransportTime3,
    MSETransportGuidByte6,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportPositionY,
    MSETransportTime2,
    MSETransportOrientation,
    MSETransportGuidByte7,

    MSETimestamp,
    MSEPitch,

    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEOrientation,
    MSEATime,
    MSESplineElevation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartTurnRightSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,

    MSECounter,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSEHasMovementFlags,
    MSEHasATime,
    MSEHasPitch,
    MSEHasTimestamp,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSEHasGuidByte5,
    MSEHasGuidByte2,
    MSEHasOrientation,
    MSEHasGuidByte1,
    MSEHasMovementFlags2,
    MSEHasGuidByte6,
    MSEHasSplineElevation,
    MSEHasFallData,
    MSEZeroBit,

    MSEHasTransportTime3,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte0,

    MSEMovementFlags2,
    MSEHasFallDirection,
    MSEMovementFlags,

    MSEGuidByte3,
    MSEGuidByte7,
    MSEGuidByte6,
    MSEGuidByte4,
    MSEGuidByte1,
    MSECounterUint32,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEGuidByte5,

    MSETransportPositionY,
    MSETransportGuidByte6,
    MSETransportPositionX,
    MSETransportPositionZ,
    MSETransportGuidByte5,
    MSETransportOrientation,
    MSETransportTime2,
    MSETransportGuidByte2,
    MSETransportGuidByte3,
    MSETransportTime3,
    MSETransportGuidByte7,
    MSETransportSeat,
    MSETransportGuidByte0,
    MSETransportGuidByte1,
    MSETransportGuidByte4,
    MSETransportTime,

    MSESplineElevation,

    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEPitch,
    MSETimestamp,
    MSEOrientation,
    MSEATime,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStopSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    MSEZeroBit,
    MSEHasGuidByte0,
    MSEHasTransportData,
    MSEHasGuidByte1,
    MSEHasGuidByte4,
    MSEHasGuidByte5,
    MSEHasGuidByte7,
    MSEHasATime,
    MSEHasGuidByte2,
    MSECounter,
    MSEHasMovementFlags,
    MSEHasOrientation,
    MSEHasPitch,
    MSEHasFallData,
    MSEHasMovementFlags2,
    MSEHasGuidByte6,
    MSEHasGuidByte3,
    MSEHasTimestamp,
    MSEHasSplineElevation,
    MSEZeroBit,
    MSEZeroBit,
    MSEMovementFlags2,

    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte2,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte5,

    MSEHasFallDirection,
    MSEMovementFlags,

    MSEGuidByte5,
    MSEGuidByte7,
    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte6,
    MSECounterUint32,
    MSEGuidByte0,
    MSEGuidByte3,
    MSEGuidByte1,

    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSETransportOrientation,
    MSETransportGuidByte1,
    MSETransportGuidByte7,
    MSETransportPositionY,
    MSETransportGuidByte6,
    MSETransportPositionX,
    MSETransportSeat,
    MSETransportGuidByte2,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportGuidByte0,
    MSETransportTime2,
    MSETransportPositionZ,
    MSETransportTime3,
    MSETransportGuidByte3,

    MSEATime,
    MSETimestamp,
    MSEPitch,
    MSEOrientation,
    MSESplineElevation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStopStrafeSequence[] =
{
    MSEPositionY,
    MSEPositionX,
    MSEPositionZ,
    MSEHasGuidByte2,
    MSEHasGuidByte4,
    MSEHasGuidByte5,
    MSEHasSplineElevation,
    MSEHasGuidByte1,
    MSECounter,
    MSEZeroBit,
    MSEHasGuidByte0,
    MSEHasOrientation,
    MSEHasGuidByte6,
    MSEZeroBit,
    MSEHasATime,
    MSEHasTimestamp,
    MSEHasPitch,
    MSEHasMovementFlags2,
    MSEHasGuidByte7,
    MSEHasGuidByte3,
    MSEHasMovementFlags,
    MSEZeroBit,
    MSEHasTransportData,
    MSEHasFallData,

    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte4,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte7,

    MSEMovementFlags,
    MSEMovementFlags2,
    MSEHasFallDirection,

    MSEGuidByte6,
    MSECounterUint32,
    MSEGuidByte7,
    MSEGuidByte1,
    MSEGuidByte5,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEGuidByte3,
    MSEGuidByte4,

    MSETransportGuidByte3,
    MSETransportSeat,
    MSETransportGuidByte5,
    MSETransportGuidByte0,
    MSETransportPositionX,
    MSETransportPositionY,
    MSETransportGuidByte2,
    MSETransportGuidByte6,
    MSETransportGuidByte1,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportTime3,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportTime2,

    MSEATime,
    MSEPitch,
    MSESplineElevation,

    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSETimestamp,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStopTurnSequence[] =
{
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEHasTimestamp,
    MSEHasGuidByte0,
    MSEHasATime,
    MSEHasGuidByte5,
    MSEZeroBit,
    MSEHasPitch,
    MSEHasOrientation,
    MSEHasGuidByte6,
    MSEHasGuidByte7,
    MSEHasMovementFlags2,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEHasGuidByte4,
    MSEHasFallData,
    MSEHasSplineElevation,
    MSECounter,
    MSEZeroBit,
    MSEHasGuidByte3,
    MSEHasTransportData,
    MSEHasGuidByte2,
    MSEHasMovementFlags,

    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte7,

    MSEHasFallDirection,
    MSEMovementFlags,
    MSEMovementFlags2,

    MSEGuidByte0,
    MSEGuidByte7,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte3,
    MSEGuidByte5,
    MSEGuidByte1,
    MSEGuidByte6,

    MSETransportGuidByte2,
    MSETransportGuidByte0,
    MSETransportSeat,
    MSETransportPositionX,
    MSETransportPositionY,
    MSETransportGuidByte3,
    MSETransportGuidByte5,
    MSETransportTime2,
    MSETransportGuidByte6,
    MSETransportTime3,
    MSETransportGuidByte1,
    MSETransportPositionZ,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportGuidByte4,
    MSETransportTime,

    MSESplineElevation,
    MSEOrientation,

    MSEFallTime,
    MSEFallVerticalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,

    MSEPitch,
    MSETimestamp,
    MSEATime,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartAscendSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEHasFallData,
    MSEZeroBit,
    MSEHasGuidByte0,
    MSEHasATime,
    MSEZeroBit,
    MSEHasGuidByte5,
    MSEHasGuidByte7,
    MSEHasMovementFlags2,
    MSEHasGuidByte6,
    MSEHasGuidByte4,
    MSEHasTimestamp,
    MSEHasTransportData,
    MSEHasMovementFlags,
    MSEHasGuidByte2,
    MSEHasSplineElevation,
    MSECounter,
    MSEHasOrientation,
    MSEHasPitch,
    MSEHasGuidByte3,

    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte4,
    MSEHasTransportTime3,

    MSEMovementFlags,
    MSEMovementFlags2,
    MSEHasFallDirection,

    MSEGuidByte6,
    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte5,
    MSECounterUint32,
    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte3,
    MSEGuidByte7,

    MSETransportGuidByte3,
    MSETransportPositionY,
    MSETransportGuidByte2,
    MSETransportTime,
    MSETransportGuidByte0,
    MSETransportPositionZ,
    MSETransportGuidByte7,
    MSETransportGuidByte5,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportPositionX,
    MSETransportOrientation,
    MSETransportGuidByte4,
    MSETransportSeat,
    MSETransportGuidByte6,
    MSETransportTime3,

    MSEPitch,
    MSESplineElevation,
    MSEATime,

    MSEFallVerticalSpeed,
    MSEFallTime,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,

    MSETimestamp,
    MSEOrientation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartDescendSequence[] =
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSEHasGuidByte3,
    MSEHasGuidByte1,
    MSEHasGuidByte6,
    MSEHasMovementFlags2,
    MSEHasSplineElevation,
    MSEHasPitch,
    MSEHasTimestamp,
    MSECounter,
    MSEHasMovementFlags,
    MSEHasGuidByte0,
    MSEHasGuidByte2,
    MSEZeroBit,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasGuidByte7,
    MSEHasTransportData,
    MSEHasATime,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSEHasOrientation,
    MSEHasGuidByte5,
    MSEHasFallDirection,

    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte0,

    MSEMovementFlags,
    MSEMovementFlags2,

    MSECounterUint32,
    MSEGuidByte3,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte6,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte1,

    MSETransportGuidByte3,
    MSETransportGuidByte5,
    MSETransportGuidByte7,
    MSETransportOrientation,
    MSETransportGuidByte6,
    MSETransportGuidByte4,
    MSETransportPositionX,
    MSETransportTime3,
    MSETransportTime,
    MSETransportGuidByte0,
    MSETransportGuidByte1,
    MSETransportSeat,
    MSETransportPositionY,
    MSETransportPositionZ,
    MSETransportTime2,
    MSETransportGuidByte2,

    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,
    MSEFallTime,

    MSEATime,
    MSETimestamp,
    MSEOrientation,
    MSESplineElevation,
    MSEPitch,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartSwimSequence[] =
{
    MSEPositionY,
    MSEPositionX,
    MSEPositionZ,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasMovementFlags2,
    MSEHasGuidByte1,
    MSEHasSplineElevation,
    MSEHasGuidByte0,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasATime,
    MSEHasFallData,
    MSEHasPitch,
    MSEHasGuidByte2,
    MSEHasOrientation,
    MSEHasMovementFlags,
    MSEHasTimestamp,
    MSEHasGuidByte7,
    MSEHasGuidByte5,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSECounter,

    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasTransportTime2,
    MSEHasTransportTime3,

    MSEHasFallDirection,
    MSEMovementFlags,
    MSEMovementFlags2,

    MSEGuidByte3,
    MSEGuidByte6,
    MSEGuidByte0,
    MSEGuidByte7,
    MSEGuidByte5,
    MSEGuidByte2,
    MSECounterUint32,
    MSEGuidByte1,
    MSEGuidByte4,

    MSETransportGuidByte2,
    MSETransportOrientation,
    MSETransportGuidByte4,
    MSETransportPositionX,
    MSETransportGuidByte6,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportGuidByte7,
    MSETransportTime,
    MSETransportTime3,
    MSETransportGuidByte5,
    MSETransportPositionZ,
    MSETransportPositionY,
    MSETransportSeat,
    MSETransportGuidByte0,
    MSETransportGuidByte3,

    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEATime,
    MSEPitch,
    MSEOrientation,
    MSETimestamp,
    MSESplineElevation,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStopSwimSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    MSEZeroBit,
    MSEHasGuidByte3,
    MSEHasSplineElevation,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasMovementFlags,
    MSEHasTimestamp,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasGuidByte1,
    MSEHasTransportData,
    MSEHasGuidByte5,
    MSEHasMovementFlags2,
    MSEHasGuidByte2,
    MSEHasATime,
    MSEHasGuidByte0,
    MSEHasOrientation,
    MSEHasPitch,
    MSEHasGuidByte7,
    MSECounter,
    MSEHasGuidByte4,

    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte0,

    MSEMovementFlags2,
    MSEHasFallDirection,
    MSEMovementFlags,

    MSEGuidByte0,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte3,
    MSEGuidByte1,
    MSEGuidByte7,
    MSEGuidByte6,
    MSEGuidByte2,

    MSETransportTime3,
    MSETransportGuidByte2,
    MSETransportGuidByte1,
    MSETransportPositionX,
    MSETransportGuidByte5,
    MSETransportGuidByte6,
    MSETransportPositionY,
    MSETransportTime2,
    MSETransportGuidByte0,
    MSETransportOrientation,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportGuidByte7,
    MSETransportSeat,
    MSETransportGuidByte3,

    MSETimestamp,

    MSEFallTime,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,

    MSESplineElevation,
    MSEOrientation,
    MSEATime,
    MSEPitch,

};

//5.4.0
MovementStatusElements const MovementStopAscendSequence[] =
{
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEHasTimestamp,
    MSEHasGuidByte5,
    MSEHasGuidByte6,
    MSEHasGuidByte7,
    MSEHasOrientation,
    MSEHasGuidByte1,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSEZeroBit,
    MSECounter,
    MSEHasFallData,
    MSEHasMovementFlags2,
    MSEHasGuidByte0,
    MSEHasATime,
    MSEHasSplineElevation,
    MSEHasGuidByte3,
    MSEHasPitch,
    MSEHasMovementFlags,
    MSEHasTransportData,
    MSEZeroBit,
    MSEHasGuidByte2,

    MSEMovementFlags2,
    MSEHasFallDirection,

    MSEHasTransportTime2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,

    MSEMovementFlags,

    MSECounterUint32,
    MSEGuidByte5,
    MSEGuidByte0,
    MSEGuidByte6,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte1,
    MSEGuidByte3,
    MSEGuidByte7,

    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSETransportPositionZ,
    MSETransportGuidByte1,
    MSETransportPositionY,
    MSETransportTime3,
    MSETransportTime,
    MSETransportOrientation,
    MSETransportGuidByte3,
    MSETransportPositionX,
    MSETransportGuidByte6,
    MSETransportGuidByte5,
    MSETransportTime2,
    MSETransportSeat,
    MSETransportGuidByte2,
    MSETransportGuidByte0,
    MSETransportGuidByte7,
    MSETransportGuidByte4,

    MSESplineElevation,
    MSEOrientation,
    MSETimestamp,
    MSEATime,
    MSEPitch,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStopPitchSequence[] =
{
	MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEHasGuidByte4,
    MSEHasGuidByte2,
    MSECounter,
    MSEHasPitch,
    MSEHasGuidByte7,
    MSEZeroBit,
    MSEHasGuidByte5,
    MSEHasGuidByte3,
    MSEZeroBit,
    MSEHasTransportData,
    MSEHasGuidByte1,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasMovementFlags,
    MSEHasFallData,
    MSEHasOrientation,
    MSEHasSplineElevation,
    MSEHasATime,
    MSEHasGuidByte0,
    MSEHasTimestamp,
    MSEHasMovementFlags2,

    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte5,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte7,

    MSEMovementFlags2,
    MSEMovementFlags,
    MSEHasFallDirection,

    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte6,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte3,

    MSEFallVerticalSpeed,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallTime,

    MSETransportGuidByte0,
    MSETransportPositionZ,
    MSETransportGuidByte2,
    MSETransportGuidByte5,
    MSETransportTime,
    MSETransportGuidByte7,
    MSETransportGuidByte6,
    MSETransportOrientation,
    MSETransportPositionX,
    MSETransportTime3,
    MSETransportPositionY,
    MSETransportSeat,
    MSETransportGuidByte3,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportGuidByte4,

    MSEOrientation,
    MSEPitch,
    MSEATime,
    MSESplineElevation,
    MSETimestamp,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartPitchDownSequence[] =
{
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEHasSplineElevation,
    MSEZeroBit,
    MSEHasOrientation,
    MSEHasPitch,
    MSEHasATime,
    MSEHasGuidByte0,
    MSEHasMovementFlags2,
    MSEHasGuidByte4,
    MSECounter,
    MSEHasGuidByte2,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasGuidByte3,
    MSEHasGuidByte6,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSEHasTimestamp,
    MSEHasMovementFlags,
    MSEHasGuidByte5,
    MSEHasGuidByte1,
    MSEHasTransportData,

    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte3,

    MSEMovementFlags2,
    MSEMovementFlags,
    MSEHasFallDirection,

    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte2,
    MSEGuidByte4,
    MSEGuidByte1,
    MSEGuidByte3,
    MSECounterUint32,

    MSETransportGuidByte7,
    MSETransportGuidByte1,
    MSETransportPositionY,
    MSETransportGuidByte6,
    MSETransportGuidByte3,
    MSETransportPositionX,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportGuidByte4,
    MSETransportTime3,
    MSETransportSeat,
    MSETransportGuidByte5,
    MSETransportTime2,
    MSETransportGuidByte2,
    MSETransportGuidByte0,
    MSETransportOrientation,

    MSEPitch,

    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallTime,
    MSEFallVerticalSpeed,

    MSEATime,
    MSEOrientation,
    MSESplineElevation,
    MSETimestamp,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MovementStartPitchUpSequence[] =
{
    MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    MSEHasMovementFlags,
    MSEHasMovementFlags2,
    MSEHasGuidByte1,
    MSEHasPitch,
    MSEHasGuidByte0,
    MSEHasSplineElevation,
    MSEHasGuidByte2,
    MSEHasFallData,
    MSEHasGuidByte5,
    MSEZeroBit,
    MSEZeroBit,
    MSEHasGuidByte7,
    MSEHasATime,
    MSEZeroBit,
    MSEHasGuidByte6,
    MSEHasTimestamp,
    MSEHasGuidByte4,
    MSEHasTransportData,
    MSEHasOrientation,
    MSECounter,
    MSEHasGuidByte3,

    MSEHasTransportGuidByte2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte3,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,

    MSEHasFallDirection,
    MSEMovementFlags2,
    MSEMovementFlags,

    MSEGuidByte6,
    MSEGuidByte5,
    MSEGuidByte1,
    MSEGuidByte7,
    MSEGuidByte4,
    MSEGuidByte0,
    MSEGuidByte3,
    MSECounterUint32,
    MSEGuidByte2,

    MSETransportGuidByte5,
    MSETransportPositionX,
    MSETransportGuidByte6,
    MSETransportGuidByte4,
    MSETransportGuidByte7,
    MSETransportPositionY,
    MSETransportGuidByte1,
    MSETransportTime3,
    MSETransportGuidByte0,
    MSETransportGuidByte2,
    MSETransportSeat,
    MSETransportGuidByte3,
    MSETransportTime2,
    MSETransportTime,
    MSETransportOrientation,
    MSETransportPositionZ,

    MSEPitch,

    MSEFallTime,
    MSEFallVerticalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,

    MSEOrientation,
    MSESplineElevation,
    MSETimestamp,
    MSEATime,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MoveChngTransport[]=
{
    MSEPositionZ,
    MSEPositionY,
    MSEPositionX,
    MSEHasGuidByte1,
    MSEHasGuidByte3,
    MSEHasGuidByte6,
    MSEHasGuidByte5,
    MSECounter,
    MSEZeroBit,
    MSEHasTimestamp,
    MSEHasFallData,
    MSEHasMovementFlags2,
    MSEHasMovementFlags,
    MSEHasATime,
    MSEHasGuidByte7,
    MSEHasSplineElevation,
    MSEHasGuidByte4,
    MSEZeroBit,
    MSEZeroBit,
    MSEHasGuidByte2,
    MSEHasTransportData,
    MSEHasOrientation,
    MSEHasGuidByte0,
    MSEHasPitch,
    MSEMovementFlags2,
    MSEMovementFlags,

    MSEHasTransportGuidByte4,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte5,

    MSEHasFallDirection,

    MSEGuidByte2,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte3,
    MSEGuidByte5,
    MSEGuidByte1,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte6,

    MSESplineElevation,

    MSETransportGuidByte1,
    MSETransportGuidByte2,
    MSETransportGuidByte6,
    MSETransportSeat,
    MSETransportPositionX,
    MSETransportGuidByte5,
    MSETransportGuidByte7,
    MSETransportGuidByte3,
    MSETransportTime3,
    MSETransportGuidByte4,
    MSETransportTime2,
    MSETransportTime,
    MSETransportGuidByte0,
    MSETransportPositionY,
    MSETransportOrientation,
    MSETransportPositionZ,

    MSEFallVerticalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallHorizontalSpeed,
    MSEFallTime,

    MSEPitch,
    MSETimestamp,
    MSEOrientation,
    MSEATime,

    MSEEnd,
};

//5.4.0
MovementStatusElements const MoveSplineDone[] =
{
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSESkipUint32,
    MSEZeroBit,
    MSEHasGuidByte5,
    MSEHasGuidByte0,
    MSEHasSplineElevation,
    MSEHasMovementFlags,
    MSEHasGuidByte1,
    MSEHasGuidByte2,
    MSEHasTimestamp,
    MSEHasOrientation,
    MSEHasGuidByte6,
    MSEHasATime,
    MSEHasPitch,
    MSEHasGuidByte7,
    MSEHasFallData,
    MSECounter,
    MSEHasMovementFlags2,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSEHasTransportData,
    MSEZeroBit,
    MSEZeroBit,
    MSEMovementFlags2,

    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasTransportTime2,
    MSEHasTransportTime3,

    MSEHasFallDirection,
    MSEMovementFlags,

    MSEGuidByte7,
    MSECounterUint32,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte1,
    MSEGuidByte4,
    MSEGuidByte0,
    MSEGuidByte2,
    MSEGuidByte3,

    MSEFallTime,
    MSEFallHorizontalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallVerticalSpeed,

    MSETransportPositionY,
    MSETransportGuidByte1,
    MSETransportGuidByte5,
    MSETransportTime3,
    MSETransportGuidByte3,
    MSETransportSeat,
    MSETransportTime2,
    MSETransportGuidByte6,
    MSETransportPositionZ,
    MSETransportOrientation,
    MSETransportTime,
    MSETransportGuidByte2,
    MSETransportGuidByte4,
    MSETransportGuidByte0,
    MSETransportGuidByte7,
    MSETransportPositionX,

    MSEOrientation,
    MSETimestamp,
    MSEATime,
    MSESplineElevation,
    MSEPitch,

    MSEEnd,
};

// 5.1.0 16357
MovementStatusElements const MoveNotActiveMover[] =
{
    MSEPositionZ,
    MSEPositionX,
    MSEPositionY,
    MSEHasMovementFlags2,
    MSEHasTransportData,
    MSEHasGuidByte6,
    MSEHasSplineElevation,
    MSEHasGuidByte3,
    MSEZeroBit,
    MSEHasTimestamp,
    MSEHasGuidByte0,
    MSEHasOrientation,
    MSEHasGuidByte5,
    MSEHasPitch,
    MSEHasGuidByte1,
    MSEHasGuidByte4,
    MSEHasGuidByte7,
    MSEHasSpline,
    MSEHasGuidByte2,
    MSEHasFallData,
    MSEHasMovementFlags,

    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte2,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte3,

    MSEHasFallDirection,
    MSEMovementFlags,
    MSEMovementFlags2,

    MSEGuidByte1,
    MSEGuidByte0,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte7,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte3,

    MSEFallVerticalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallTime,

    MSETransportTime3,
    MSETransportGuidByte1,
    MSETransportTime2,
    MSETransportOrientation,
    MSETransportGuidByte0,
    MSETransportSeat,
    MSETransportGuidByte4,
    MSETransportGuidByte6,
    MSETransportGuidByte3,
    MSETransportGuidByte5,
    MSETransportPositionY,
    MSETransportPositionX,
    MSETransportGuidByte2,
    MSETransportPositionZ,
    MSETransportGuidByte7,
    MSETransportTime,

    MSETimestamp,
    MSESplineElevation,
    MSEPitch,
    MSEOrientation,

    MSEEnd,
};
// 5.4.0
MovementStatusElements const MoveSetCanFly[] =
{
    MSEHasGuidByte0,
    MSEHasGuidByte1,
    MSEHasGuidByte6,
    MSEHasGuidByte5,
    MSEHasGuidByte7,
    MSEHasGuidByte2,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSECounterUint32,
    MSEGuidByte4,
    MSEGuidByte3,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEGuidByte6,
    MSEEnd,
};
// 5.4.0
MovementStatusElements const MoveUnsetCanFly[] =
{
    MSEHasGuidByte2,
    MSEHasGuidByte1,
    MSEHasGuidByte5,
    MSEHasGuidByte0,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSEHasGuidByte6,
    MSEHasGuidByte7,
    MSEGuidByte1,
    MSEGuidByte5,
    MSEGuidByte6,
    MSECounterUint32,
    MSEGuidByte7,
    MSEGuidByte4,
    MSEGuidByte2,
    MSEGuidByte3,
    MSEGuidByte0,
    MSEEnd,
};

// 4.3.4
MovementStatusElements const DismissControlledVehicle[] =
{
     MSEPositionX,
    MSEPositionZ,
    MSEPositionY,
    //MSEHasAliveTime,
    MSEHasGuidByte4,
    MSEHasGuidByte1,
    MSEHasGuidByte2,
    MSEHasGuidByte6,
    MSEZeroBit,
    MSEZeroBit,
    MSEHasTimestamp,
    MSEHasSplineElevation,
    MSEHasOrientation,
    MSEHasPitch,
    MSECounter,
    MSEHasMovementFlags2,
    MSEHasGuidByte3,
    MSEHasGuidByte7,
    MSEHasTransportData,
    MSEHasGuidByte0,
    MSEHasMovementFlags,
    MSEZeroBit,
    MSEHasFallData,
    MSEHasGuidByte5,

    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte6,
    MSEHasTransportTime2,
    MSEHasTransportTime3,

    MSEMovementFlags,
    MSEHasFallDirection,
    MSEMovementFlags2,

    MSEGuidByte4,
    MSECounterUint32,
    MSEGuidByte6,
    MSEGuidByte3,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEGuidByte1,

    MSETransportOrientation,
    MSETransportGuidByte0,
    MSETransportSeat,
    MSETransportGuidByte1,
    MSETransportGuidByte4,
    MSETransportTime3,
    MSETransportGuidByte5,
    MSETransportTime2,
    MSETransportGuidByte3,
    MSETransportGuidByte6,
    MSETransportPositionY,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportGuidByte7,
    MSETransportGuidByte2,
    MSETransportPositionX,

    MSEFallTime,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSEFallVerticalSpeed,

    //MSEAliveTime,
    MSESplineElevation,
    MSEOrientation,
    MSEPitch,
    MSETimestamp,

    MSEEnd,
};

//5.1.0 16357 OK
MovementStatusElements const MoveUpdateTeleport[] =
{
    MSEHasSpline,
    MSEHasGuidByte5,
    MSEHasSplineElevation,
    MSEHasGuidByte2,
    MSEHasGuidByte1,
    MSEHasFallData,
    MSEHasTimestamp,
    MSEHasGuidByte4,
    //MSEHasCounter,
    MSEHasMovementFlags,
    MSEHasGuidByte6,
    MSEHasOrientation,
    MSEZeroBit,
    MSEHasFallDirection,
    MSEHasTransportData,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte5,
    MSEHasTransportTime3,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte1,
    MSEHasTransportTime2,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte4,
    MSEHasATime,
    MSEHasMovementFlags2,
    MSEHasGuidByte3,
    MSEMovementFlags2,
    MSEMovementFlags,
    MSEHasPitch,
    MSEHasGuidByte7,
    MSEHasGuidByte0,
    MSEZeroBit,
    MSEPositionX,
    MSETransportGuidByte3,
    MSETransportTime2,
    MSETransportGuidByte0,
    MSETransportTime3,
    MSETransportTime,
    MSETransportGuidByte5,
    MSETransportPositionY,
    MSETransportPositionX,
    MSETransportOrientation,
    MSETransportPositionZ,
    MSETransportGuidByte4,
    MSETransportGuidByte2,
    MSETransportGuidByte7,
    MSETransportSeat,
    MSETransportGuidByte6,
    MSETransportGuidByte1,
    MSEPositionY,
    MSECounter,
    MSETimestamp,
    MSEGuidByte1,
    MSEFallHorizontalSpeed,
    MSEFallSinAngle,
    MSEFallCosAngle,
    MSEFallVerticalSpeed,
    MSEFallTime,
    MSEGuidByte3,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte7,
    MSEPositionZ,
    MSEPitch,
    MSEOrientation,
    MSEGuidByte6,
    MSEGuidByte2,
    MSEGuidByte0,
    MSEATime,
    MSESplineElevation,
    MSEEnd,
};

MovementStatusElements const MoveCastSpell[] =
{
    MSEPositionZ,
    MSEPositionX,
	MSEPositionY,
	MSEHasGuidByte7,
    MSEHasSpline,
	MSEHasFallData,
    MSEHasGuidByte0,
	MSEHasMovementFlags,
	MSEHasTransportData,
	MSEHasGuidByte2,
	MSEZeroBit,
	MSEHasMovementFlags2,
	MSEHasGuidByte1,
	MSEHasATime,
	MSEHasGuidByte3,
	MSEHasOrientation,
	MSEHasTimestamp,
    //MSEHasCounter,
    MSEHasGuidByte5,
	MSEHasGuidByte6,
	MSEHasPitch,
	MSEHasGuidByte4,
    MSEHasSplineElevation, 
    MSEZeroBit,
	MSEHasTransportTime2,
	MSEHasTransportGuidByte2,
	MSEHasTransportGuidByte3,
	MSEHasTransportGuidByte4,
	MSEHasTransportGuidByte0,
	MSEHasTransportGuidByte5,
    MSEHasTransportTime3,
	MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte7,
    MSEHasTransportGuidByte1,
    MSEMovementFlags2,
    MSEHasFallDirection,
    MSEMovementFlags,
	MSEGuidByte1,
	MSEGuidByte2,
	MSEGuidByte5,
	MSECounter,
	MSEGuidByte6,
	MSEGuidByte7,
	MSEGuidByte4,
	MSEGuidByte0,
	MSEGuidByte3,
    MSETransportTime,
    MSETransportGuidByte5,
    MSETransportSeat,
	MSETransportOrientation,
	MSETransportGuidByte7,
	MSETransportGuidByte1,
	MSETransportPositionZ,
	MSETransportPositionX,
	MSETransportGuidByte6,
	MSETransportPositionY,
	MSETransportGuidByte4,
    MSETransportGuidByte2,
	MSETransportTime3,
	MSETransportTime2,
	MSETransportGuidByte0,
    MSETransportGuidByte3,
    MSEATime,
    MSEOrientation,
	MSEFallTime,
	MSEFallCosAngle,
	MSEFallHorizontalSpeed,
    MSEFallSinAngle,
	MSEFallVerticalSpeed,
    MSETimestamp,
    MSEPitch,
    MSESplineElevation,
    MSEEnd,
};

// 5.4.0
MovementStatusElements const MoveSetFlightSpeed[] =
{
    MSEHasGuidByte1,
    MSEHasGuidByte5,
    MSEHasGuidByte4,
    MSEHasGuidByte0,
    MSEHasGuidByte6,
    MSEHasGuidByte3,
    MSEHasGuidByte2,
    MSEHasGuidByte7,
    MSEGuidByte5,
    MSEGuidByte1,
    MSEGuidByte3,
    MSEGuidByte7,
    MSEGuidByte0,
    MSEGuidByte2,
    MSECounterUint32,
    MSEGuidByte6,
    MSEGuidByte4,
    MSEExtraElement,
    MSEEnd,
};

// 5.4.0
MovementStatusElements const MoveSetRunSpeed[] =
{
    MSEHasGuidByte6,
    MSEHasGuidByte5,
    MSEHasGuidByte0,
    MSEHasGuidByte1,
    MSEHasGuidByte2,
    MSEHasGuidByte4,
    MSEHasGuidByte3,
    MSEHasGuidByte7,
    MSEGuidByte4,
    MSEGuidByte7,
    MSEGuidByte1,
    MSEGuidByte3,
    MSEExtraElement,
    MSEGuidByte0,
    MSEGuidByte5,
    MSEGuidByte2,
    MSEGuidByte6,
    MSECounterUint32,
    MSEEnd,
};

// 5.4.0
MovementStatusElements const MoveSetSwimSpeed[] =
{
    MSEHasGuidByte0,
    MSEHasGuidByte5,
    MSEHasGuidByte2,
    MSEHasGuidByte6,
    MSEHasGuidByte7,
    MSEHasGuidByte4,
    MSEHasGuidByte1,
    MSEHasGuidByte3,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte3,
    MSECounterUint32,
    MSEGuidByte0,
    MSEGuidByte6,
    MSEGuidByte2,
    MSEGuidByte1,
    MSEGuidByte7,
    MSEExtraElement,
    MSEEnd,
};

void Movement::ExtraMovementStatusElement::ReadNextElement(ByteBuffer& packet)
{
    MovementStatusElements const element = _elements[_index++];

    switch (element)
    {
        case MSEHasGuidByte0:
        case MSEHasGuidByte1:
        case MSEHasGuidByte2:
        case MSEHasGuidByte3:
        case MSEHasGuidByte4:
        case MSEHasGuidByte5:
        case MSEHasGuidByte6:
        case MSEHasGuidByte7:
            Data.guid[element - MSEHasGuidByte0] = packet.ReadBit();
            break;
        case MSEGuidByte0:
        case MSEGuidByte1:
        case MSEGuidByte2:
        case MSEGuidByte3:
        case MSEGuidByte4:
        case MSEGuidByte5:
        case MSEGuidByte6:
        case MSEGuidByte7:
            packet.ReadByteSeq(Data.guid[element - MSEGuidByte0]);
            break;
        case MSEExtraFloat:
            packet >> Data.floatData;
            break;
        case MSEExtraInt8:
            packet >> Data.byteData;
            break;
        default:
            ASSERT(PrintInvalidSequenceElement(element, __FUNCTION__));
            break;
    }
}

void Movement::ExtraMovementStatusElement::WriteNextElement(ByteBuffer& packet)
{
    MovementStatusElements const element = _elements[_index++];

    switch (element)
    {
        case MSEHasGuidByte0:
        case MSEHasGuidByte1:
        case MSEHasGuidByte2:
        case MSEHasGuidByte3:
        case MSEHasGuidByte4:
        case MSEHasGuidByte5:
        case MSEHasGuidByte6:
        case MSEHasGuidByte7:
            packet.WriteBit(Data.guid[element - MSEHasGuidByte0]);
            break;
        case MSEGuidByte0:
        case MSEGuidByte1:
        case MSEGuidByte2:
        case MSEGuidByte3:
        case MSEGuidByte4:
        case MSEGuidByte5:
        case MSEGuidByte6:
        case MSEGuidByte7:
            packet.WriteByteSeq(Data.guid[element - MSEGuidByte0]);
            break;
        case MSEExtraFloat:
            packet << Data.floatData;
            break;
        case MSEExtraInt8:
            packet << Data.byteData;
            break;
        default:
            ASSERT(PrintInvalidSequenceElement(element, __FUNCTION__));
            break;
    }
}

bool Movement::PrintInvalidSequenceElement(MovementStatusElements const element, char const* function)
{
    sLog->outError(LOG_FILTER_UNITS, "Incorrect sequence element %d detected at %s", element, function);
    return false;
}

Movement::PacketSender::PacketSender(Unit* unit, Opcodes serverControl, Opcodes playerControl, Opcodes broadcast /*= SMSG_PLAYER_MOVE*/, ExtraMovementStatusElement* extras /*= NULL*/)
    : _extraElements(extras), _unit(unit)
{
    if (unit->GetTypeId() == TYPEID_PLAYER && unit->ToPlayer()->m_mover->GetTypeId() == TYPEID_PLAYER)
    {
        _selfOpcode = playerControl;
        _broadcast = broadcast;
    }
    else
    {
        _selfOpcode = NULL_OPCODE;
        _broadcast = serverControl;
    }
}

void Movement::PacketSender::Send() const
{
    bool isPlayerMovement = false;
    if (Player* player = _unit->ToPlayer())
    {
        isPlayerMovement = player->m_mover->GetTypeId() == TYPEID_PLAYER;
        if (isPlayerMovement && _selfOpcode != NULL_OPCODE)
        {
            WorldPacket data(_selfOpcode);
            _unit->WriteMovementInfo(data, _extraElements);
            player->SendDirectMessage(&data);
        }
    }

    if (_broadcast != NULL_OPCODE)
    {
        ///! Need to reset current extra element index before writing another packet
        if (_extraElements)
            _extraElements->ResetIndex();

        WorldPacket data(_broadcast);
        _unit->WriteMovementInfo(data, _extraElements);
        _unit->SendMessageToSet(&data, !isPlayerMovement);
    }
}

MovementStatusElements const* GetMovementStatusElementsSequence(Opcodes opcode)
{
    switch (opcode)
    {
    case CMSG_MOVE_FALL_RESET:
        return MovementFallResetSequence;
    case MSG_MOVE_FALL_LAND:
        return MovementFallLandSequence;
    case MSG_MOVE_HEARTBEAT:
        return MovementHeartBeatSequence;
    case MSG_MOVE_JUMP:
        return MovementJumpSequence;
    case MSG_MOVE_SET_FACING:
        return MovementSetFacingSequence;
    case MSG_MOVE_SET_PITCH:
        return MovementSetPitchSequence;
    case MSG_MOVE_START_ASCEND:
        return MovementStartAscendSequence;
    case MSG_MOVE_START_BACKWARD:
        return MovementStartBackwardSequence;
    case MSG_MOVE_START_DESCEND:
        return MovementStartDescendSequence;
    case MSG_MOVE_START_FORWARD:
        return MovementStartForwardSequence;
    case MSG_MOVE_START_PITCH_DOWN:
        return MovementStartPitchDownSequence;
    case MSG_MOVE_START_PITCH_UP:
        return MovementStartPitchUpSequence;
    case MSG_MOVE_START_STRAFE_LEFT:
        return MovementStartStrafeLeftSequence;
    case MSG_MOVE_START_STRAFE_RIGHT:
        return MovementStartStrafeRightSequence;
    case MSG_MOVE_START_SWIM:
        return MovementStartSwimSequence;
    case MSG_MOVE_START_TURN_LEFT:
        return MovementStartTurnLeftSequence;
    case MSG_MOVE_START_TURN_RIGHT:
        return MovementStartTurnRightSequence;
    case MSG_MOVE_STOP:
        return MovementStopSequence;
    case MSG_MOVE_STOP_ASCEND:
        return MovementStopAscendSequence;
    case MSG_MOVE_STOP_PITCH:
        return MovementStopPitchSequence;
    case MSG_MOVE_STOP_STRAFE:
        return MovementStopStrafeSequence;
    case MSG_MOVE_STOP_SWIM:
        return MovementStopSwimSequence;
    case MSG_MOVE_STOP_TURN:
        return MovementStopTurnSequence;
    case SMSG_PLAYER_MOVE:
        return PlayerMoveSequence;
    case CMSG_MOVE_CHNG_TRANSPORT:
        return MoveChngTransport;
    case CMSG_MOVE_SPLINE_DONE:
        return MoveSplineDone;
    case CMSG_MOVE_NOT_ACTIVE_MOVER:
        return MoveNotActiveMover;
	case SMSG_MOVE_SET_CAN_FLY:
            return MoveSetCanFly;
    case SMSG_MOVE_UNSET_CAN_FLY:
            return MoveUnsetCanFly;
    case CMSG_DISMISS_CONTROLLED_VEHICLE:
        return DismissControlledVehicle;
    case SMSG_MOVE_UPDATE_TELEPORT:
        return MoveUpdateTeleport;
	case CMSG_CAST_SPELL:
		return MoveCastSpell;
	case SMSG_MOVE_SET_FLIGHT_SPEED:
         return MoveSetFlightSpeed;
	case SMSG_MOVE_SET_RUN_SPEED:
        return MoveSetRunSpeed;
	case SMSG_MOVE_SET_SWIM_SPEED: // Still need more
        return MoveSetSwimSpeed;
    default:
        break;
    }

    return NULL;
}

