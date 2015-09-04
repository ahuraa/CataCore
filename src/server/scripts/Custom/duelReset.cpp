/*
TER-Server
*/

#include "ScriptPCH.h"
#include "Config.h"
#include "World.h"

class DuelResetCooldown : public PlayerScript
{
    public:
        DuelResetCooldown() : PlayerScript("DuelResetCooldown") {}

    void OnDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
    {
        // reset cooldowns in Elewynn Forest and Durotar
        if ((ConfigMgr::GetIntDefault("DuelResetCooldown.Enable", 1)) && 
        (winner->GetAreaId() == (ConfigMgr::GetIntDefault("DuelReset.Area.One", NULL)) || winner->GetAreaId() == (ConfigMgr::GetIntDefault("DuelReset.Area.Two", NULL))))
        {
            winner->ResetAllPowers();
            winner->RemoveArenaSpellCooldowns(true);
            loser->ResetAllPowers();
            loser->RemoveArenaSpellCooldowns(true);
        }
    }
};

void AddSC_DuelReset()
{
    new DuelResetCooldown;
}
