#include "heroes/BountyHunter/MarkForDeath.h"
#include "generators/EffectGenerator.h"
#include "effects/Damage.h"
#include "effects/Mark.h"
#include "effects/Bleed.h"
#include "effects/Buff.h"
#include "effects/Debuff.h"

namespace Heroes {
    namespace BountyHunter {
        MarkForDeath::MarkForDeath() : skillDesigns::RangeSkill(1, "MarkForDeath", {1, 2, 3, 4}, {}, {1, 2, 3, 4}, 100, 0) {}

        void MarkForDeath::unsafeTargetUse(int crited, std::shared_ptr<BattleField> battleField,
                                    std::shared_ptr<entity::Entity> actor,
                                    std::shared_ptr<entity::Entity> object) {

            Skill::addEffect(object, generators::EffectGenerator::generateHeroDamage<effects::Damage>
            (crited, actor, -100), battleField, crited, 100);

            Skill::addEffect(actor, generators::EffectGenerator::generateNumberOfTurnsEffect<effects::Buff>
            (crited, 2, std::map<int, int>{{static_cast<int>(Characteristic::speed), 3}}), battleField, crited, 100);

            Skill::addEffect(object, generators::EffectGenerator::generateNumberOfTurnsEffect<effects::Debuff>
            (crited, 3, std::map<int, int>{{static_cast<int>(Characteristic::marked), 1},
            {static_cast<int>(Characteristic::defence), -10}}), battleField, crited, 100);

        }

        void MarkForDeath::unsafeSelfUse(int crited, std::shared_ptr<BattleField> battleField,
                                  std::shared_ptr<entity::Entity> object) {

        }
        std::string MarkForDeath::getEffectDescription() {
            return "Damage -100% Mark Target (3 rds) -10% PROT (100% base, 3 rds)";
        }
        std::string MarkForDeath::getSelfDescription() {
            return "";
        }
    } // namespace BrigandFusilier
} // namespace enemies