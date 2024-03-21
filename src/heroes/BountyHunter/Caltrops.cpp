#include "heroes/BountyHunter/Caltrops.h"
#include "generators/EffectGenerator.h"
#include "effects/Damage.h"
#include "effects/Mark.h"
#include "effects/Bleed.h"
#include "effects/Debuff.h"
namespace Heroes {
    namespace BountyHunter {
        Caltrops::Caltrops() : skillDesigns::RangeSkill(1, "Caltrops", {1, 2, 3}, {}, {3, 4}, 90, 5) {}

        void Caltrops::unsafeTargetUse(int crited, std::shared_ptr<BattleField> battleField,
                                    std::shared_ptr<entity::Entity> actor,
                                    std::shared_ptr<entity::Entity> object) {
            Skill::addEffect(object, generators::EffectGenerator::generateHeroDamage<effects::Damage>
            (crited, actor, -95), battleField, crited, 100);
            Skill::addEffect(object, generators::EffectGenerator::generateNumberOfTurnsEffect<effects::Bleed>
            (crited, 3, 2), battleField, crited, 100);
            Skill::addEffect(object, generators::EffectGenerator::generateNumberOfTurnsEffect<effects::Debuff>
            (crited, 1, std::map<int, int>{{static_cast<int>(Characteristic::defence), -10}}), battleField, crited, 100);
            Skill::addEffect(object, generators::EffectGenerator::generateNumberOfTurnsEffect<effects::Debuff>
            (crited, 3, std::map<int, int>{{static_cast<int>(Characteristic::speed), -4}}), battleField, crited, 100);
        }

        void Caltrops::unsafeSelfUse(int crited, std::shared_ptr<BattleField> battleField,
                                  std::shared_ptr<entity::Entity> object) {

        }

        std::string Caltrops::getEffectDescription() {
            return "Damage -95% Bleed (100% base) 2 pts/rd for 3 rds +10% DMG Taken (100% base) -4 SPD (100% base) (3 rds)";
        }
        std::string Caltrops::getSelfDescription() {
            return "";
        }
    } // namespace BountyHunter
} // namespace enemies