/*
TER-Server
*/

#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"

void WorldSession::SendAuthResponse(uint8 code, bool queued, uint32 queuePos)
{
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1 /*bits*/ + 4 + 1 + 4 + 1 + 4 + 1 + 1 + (queued ? 4 : 0));
    packet.WriteBit(queued);
    if (queued)
        packet.WriteBit(0);

    packet.WriteBit(1);                                    // has account info

    packet.FlushBits();

    // account info
    packet << uint32(0);                                   // BillingTimeRemaining
    packet << uint8(Expansion());                          // 0 - normal, 1 - TBC, 2 - WOTLK, 3 - CATA; must be set in database manually for each account
    packet << uint32(0);
    packet << uint8(Expansion());                          // Unknown, these two show the same
    packet << uint32(0);                                   // BillingTimeRested
    packet << uint8(0);                                    // BillingPlanFlags

    packet << uint8(code);
    if (queued)
        packet << uint32(queuePos);                             // Queue position

    SendPacket(&packet);
}

void WorldSession::SendClientCacheVersion(uint32 version)
{
    WorldPacket data(SMSG_CLIENTCACHE_VERSION, 4);
    data << uint32(version);
    SendPacket(&data);
}
