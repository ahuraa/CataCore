/*
TER-Server
*/

#ifndef TRINITY_PETAI_H
#define TRINITY_PETAI_H

#include "CreatureAI.h"
#include "Timer.h"

class Creature;
class Spell;

class PetAI : public CreatureAI
{
    public:

        explicit PetAI(Creature* c);

        void UpdateAI(const uint32);
        static int Permissible(const Creature*);

        void KilledUnit(Unit* /*victim*/);
        void AttackStart(Unit* target);
        void AttackStart(Unit* target, uint32 spellId);
        void MovementInform(uint32 moveType, uint32 data);
        void OwnerAttackedBy(Unit* attacker);
        void OwnerAttacked(Unit* target);
        void AttackedBy(Unit* attacker);
        void ReceiveEmote(Player* player, uint32 textEmote);
        void HandleReturnMovement();
        bool IsCasterPet();
        float GetAttackDistance(Unit* victim = NULL);

        // The following aren't used by the PetAI but need to be defined to override
        //  default CreatureAI functions which interfere with the PetAI
        //
        void MoveInLineOfSight(Unit* /*who*/) {} // CreatureAI interferes with returning pets
        void MoveInLineOfSight_Safe(Unit* /*who*/) {} // CreatureAI interferes with returning pets
        void EnterEvadeMode() {} // For fleeing, pets don't use this type of Evade mechanic

    private:
        bool _isVisible(Unit*) const;
        bool _needToStop(void);
        void _stopAttack(void);

        void UpdateAllies();

        TimeTracker i_tracker;
        bool inCombat;
        std::set<uint64> m_AllySet;
        uint32 m_updateAlliesTimer;

        Unit* SelectNextTarget(bool allowAutoSelect) const;
        void DoAttack(Unit* target, bool chase, uint32 spellId = 0);
        bool CanAttack(Unit* target);
        void ClearCharmInfoFlags();
};
#endif

