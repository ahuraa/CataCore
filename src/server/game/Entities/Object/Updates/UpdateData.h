/*
TER-Server
*/

#ifndef __UPDATEDATA_H
#define __UPDATEDATA_H

#include "ByteBuffer.h"
class WorldPacket;

enum OBJECT_UPDATE_TYPE
{
    UPDATETYPE_VALUES               = 0,
    UPDATETYPE_CREATE_OBJECT        = 1,
    UPDATETYPE_CREATE_OBJECT2       = 2,
    UPDATETYPE_OUT_OF_RANGE_OBJECTS = 3,
};

enum OBJECT_UPDATE_FLAGS
{
    UPDATEFLAG_NONE                  = 0x0000,
    UPDATEFLAG_SELF                  = 0x0001,
    UPDATEFLAG_TRANSPORT             = 0x0002,
    UPDATEFLAG_HAS_TARGET            = 0x0004,
    UPDATEFLAG_UNKNOWN               = 0x0008,
    UPDATEFLAG_LOWGUID               = 0x0010,
    UPDATEFLAG_LIVING                = 0x0020,
    UPDATEFLAG_STATIONARY_POSITION   = 0x0040,
    UPDATEFLAG_VEHICLE               = 0x0080,
    UPDATEFLAG_GO_TRANSPORT_POSITION = 0x0100,
    UPDATEFLAG_ROTATION              = 0x0200,
    UPDATEFLAG_UNK3                  = 0x0400,
    UPDATEFLAG_ANIMKITS              = 0x0800,
    UPDATEFLAG_UNK5                  = 0x1000,
    UPDATEFLAG_UNK6                  = 0x2000,
};

class UpdateData
{
    public:
        UpdateData(uint16 map);

        void AddOutOfRangeGUID(std::set<uint64>& guids);
        void AddOutOfRangeGUID(uint64 guid);
        void AddUpdateBlock(const ByteBuffer &block);
        bool BuildPacket(WorldPacket* packet);
        bool HasData() const { return m_blockCount > 0 || !m_outOfRangeGUIDs.empty(); }
        void Clear();

        std::set<uint64> const& GetOutOfRangeGUIDs() const { return m_outOfRangeGUIDs; }

    protected:
        uint16 m_map;
        uint32 m_blockCount;
        std::set<uint64> m_outOfRangeGUIDs;
        ByteBuffer m_data;
};
#endif

