/*
TER-Server
*/

#ifndef _MOVEMENT_STRUCTURES_H
#define _MOVEMENT_STRUCTURES_H

#include "MovementInfo.h"
#include "Opcodes.h"

enum MovementStatusElements
{
    MSEHasGuidByte0,
    MSEHasGuidByte1,
    MSEHasGuidByte2,
    MSEHasGuidByte3,
    MSEHasGuidByte4,
    MSEHasGuidByte5,
    MSEHasGuidByte6,
    MSEHasGuidByte7,
    MSEHasMovementFlags,
    MSEHasMovementFlags2,
    MSEHasTimestamp,
    MSEHasOrientation,
    MSEHasTransportData,
    MSEHasTransportGuidByte0,
    MSEHasTransportGuidByte1,
    MSEHasTransportGuidByte2,
    MSEHasTransportGuidByte3,
    MSEHasTransportGuidByte4,
    MSEHasTransportGuidByte5,
    MSEHasTransportGuidByte6,
    MSEHasTransportGuidByte7,
    MSEHasTransportTime2,
    MSEHasTransportTime3,
    MSEHasPitch,
    MSEHasFallData,
    MSEHasFallDirection,
    MSEHasSplineElevation,
    MSEHasSpline,

    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte2,
    MSEGuidByte3,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte7,
    MSEMovementFlags,
    MSEMovementFlags2,
    MSETimestamp,
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEOrientation,
    MSETransportGuidByte0,
    MSETransportGuidByte1,
    MSETransportGuidByte2,
    MSETransportGuidByte3,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportGuidByte6,
    MSETransportGuidByte7,
    MSETransportPositionX,
    MSETransportPositionY,
    MSETransportPositionZ,
    MSETransportOrientation,
    MSETransportSeat,
    MSETransportTime,
    MSETransportTime2,
    MSETransportTime3,
    MSEPitch,
    MSEFallTime,
    MSEFallVerticalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSEFallHorizontalSpeed,
    MSESplineElevation,

    MSECounter,

    // Speed
    MSESpeedWalk,
    MSESpeedRun,
    MSESpeedRunBack,
    MSESpeedSwim,
    MSESpeedSwimBack,
    MSESpeedTurnRate,
    MSESpeedFlight,
    MSESpeedFlightBack,
    MSESpeedPitchRate,

    MSEHeight,

    // Special
    MSEZeroBit, // writes bit value 0 or skips read bit
    MSEOneBit,  // writes bit value 1 or skips read bit
    MSEEnd,     // marks end of parsing
    MSE_COUNT,
	MSEExtraFloat
};

namespace Movement
{
    class PacketSender
    {
    public:
        PacketSender(Unit* unit, Opcodes controlled, Opcodes self, MovementInfo* info, bool broadCastSelf = false, bool broadCastControlled = true);

        void Send() const;

    private:
        Unit* _unit;
        Opcodes _toSend;
        bool _BroadCastControlled, _BroadCastSelf, _isControlledPacket;
        MovementInfo* _info;
    };

	class ExtraMovementStatusElement
	{
		friend class PacketSender;

	public:
		ExtraMovementStatusElement(MovementStatusElements const* elements) : _elements(elements), _index(0) { }

		void ReadNextElement(ByteBuffer& packet);
		void WriteNextElement(ByteBuffer& packet);

		struct
		{
			ObjectGuid guid;
			float floatData;
			int8  byteData;
		} Data;

	protected:
		void ResetIndex() { _index = 0; }

	private:
		MovementStatusElements const* _elements;
		uint32 _index;
	};
}

MovementStatusElements* GetMovementStatusElementsSequence(Opcodes opcode);

#endif
