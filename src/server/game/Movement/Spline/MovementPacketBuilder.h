/*
TER-Server
*/

#ifndef TRINITYSERVER_PACKET_BUILDER_H
#define TRINITYSERVER_PACKET_BUILDER_H

#include "Define.h" // for uint32
#include "G3D/Vector3.h"
using G3D::Vector3;

class ByteBuffer;
class WorldPacket;

namespace Movement
{
    class MoveSpline;
    class PacketBuilder
    {
        static void WriteCommonMonsterMovePart(const MoveSpline& mov, WorldPacket& data);
    public:

        static void WriteMonsterMove(const MoveSpline& mov, WorldPacket& data);
        static void WriteStopMovement(Vector3 const& loc, uint32 splineId, ByteBuffer& data);
        static void WriteCreateBits(MoveSpline const& moveSpline, ByteBuffer& data);
        static void WriteCreateData(MoveSpline const& moveSpline, ByteBuffer& data);
    };
}
#endif // TRINITYSERVER_PACKET_BUILDER_H
