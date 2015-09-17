/*
TER-Server
*/

#ifndef __TRINITY_VEHICLEDEFINES_H
#define __TRINITY_VEHICLEDEFINES_H

#include "Define.h"
#include <vector>
#include <map>

struct VehicleSeatEntry;

enum PowerType
{
    POWER_STEAM                                  = 61,
    POWER_PYRITE                                 = 41,
    POWER_HEAT                                   = 101,
    POWER_OOZE                                   = 121,
    POWER_BLOOD                                  = 141,
    POWER_WRATH                                  = 142,
    POWER_ARCANE_ENERGY                          = 143,
    POWER_LIFE_ENERGY                            = 144,
    POWER_SUN_ENERGY                             = 145,
    POWER_SWING_VELOCITY                         = 146,
    POWER_SHADOWFLAME_ENERGY                     = 147,
    POWER_BLUE_POWER                             = 148,
    POWER_PURPLE_POWER                           = 149,
    POWER_GREEN_POWER                            = 150,
    POWER_ORANGE_POWER                           = 151,
    POWER_ENERGY_2                               = 153,
    POWER_ARCANEENERGY                           = 161,
    POWER_WIND_POWER_1                           = 162,
    POWER_WIND_POWER_2                           = 163,
    POWER_WIND_POWER_3                           = 164,
    POWER_FUEL                                   = 165,
    POWER_SUN_POWER                              = 166,
    POWER_TWILIGHT_ENERGY                        = 169,
    POWER_VENOM                                  = 174,
    POWER_ORANGE_POWER_2                         = 176,
    POWER_CONSUMING_FLAME                        = 177,
    POWER_PYROCLASTIC_FRENZY                     = 178,
    POWER_FLASHFIRE                              = 179,
};

enum VehicleFlags
{
    VEHICLE_FLAG_NO_STRAFE                       = 0x00000001,           // Sets MOVEFLAG2_NO_STRAFE
    VEHICLE_FLAG_NO_JUMPING                      = 0x00000002,           // Sets MOVEFLAG2_NO_JUMPING
    VEHICLE_FLAG_FULLSPEEDTURNING                = 0x00000004,           // Sets MOVEFLAG2_FULLSPEEDTURNING
    VEHICLE_FLAG_ALLOW_PITCHING                  = 0x00000010,           // Sets MOVEFLAG2_ALLOW_PITCHING
    VEHICLE_FLAG_FULLSPEEDPITCHING               = 0x00000020,           // Sets MOVEFLAG2_FULLSPEEDPITCHING
    VEHICLE_FLAG_CUSTOM_PITCH                    = 0x00000040,           // If set use pitchMin and pitchMax from DBC, otherwise pitchMin = -pi/2, pitchMax = pi/2
    VEHICLE_FLAG_ADJUST_AIM_ANGLE                = 0x00000400,           // Lua_IsVehicleAimAngleAdjustable
    VEHICLE_FLAG_ADJUST_AIM_POWER                = 0x00000800            // Lua_IsVehicleAimPowerAdjustable
};

enum VehicleSpells
{
    VEHICLE_SPELL_RIDE_HARDCODED                 = 46598,
    VEHICLE_SPELL_PARACHUTE                      = 45472
};

struct VehicleSeat
{
    explicit VehicleSeat(VehicleSeatEntry const* seatInfo) : SeatInfo(seatInfo), Passenger(0) {}
    VehicleSeatEntry const* SeatInfo;
    uint64 Passenger;
};

struct VehicleAccessory
{
    VehicleAccessory(uint32 entry, int8 seatId, bool isMinion, uint8 summonType, uint32 summonTime) :
        AccessoryEntry(entry), IsMinion(isMinion), SummonTime(summonTime), SeatId(seatId), SummonedType(summonType) {}
    uint32 AccessoryEntry;
    uint32 IsMinion;
    uint32 SummonTime;
    int8 SeatId;
    uint8 SummonedType;
};

typedef std::vector<VehicleAccessory> VehicleAccessoryList;
typedef std::map<uint32, VehicleAccessoryList> VehicleAccessoryContainer;
typedef std::map<int8, VehicleSeat> SeatMap;

class TransportBase
{
    public:
        /// This method transforms supplied transport offsets into global coordinates
        virtual void CalculatePassengerPosition(float& x, float& y, float& z, float& o) = 0;

        /// This method transforms supplied global coordinates into local offsets
        virtual void CalculatePassengerOffset(float& x, float& y, float& z, float& o) = 0;
};

#endif
