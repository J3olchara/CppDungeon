#include "BattleField.h"
#include "changers/ActionsChanger.h"
#include "entity/Enemy.h"
#include "entity/MarkedAsAutoTurn.h"
#include "events/EnemyEncounter.h"
#include "entity/Hero.h"
#include "enemies/BrigandRaider/BrigandRaider.h"
#include "enemies/BrigandFusier/BrigandFusilier.h"
#include "enemies/CultistAcolyte/CultistAcolyte.h"
#include "navigation/Map.h"
#include "monitor/Monitor.h"
#include "player/Player.h"
#include "actions/UseSkill.h"
#include "actions/UseItem.h"
#include "generators/NumberGenerator.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdlib>

namespace events {
    EnemyEncounter::EnemyEncounter() {

        std::vector<std::shared_ptr<entity::Entity>> tmpEnemies;
        int numberOfEnemies = generators::NumberGenerator::generate(1, 3);
        tmpEnemies.resize(numberOfEnemies);

        for (auto &i: tmpEnemies) {
            int num = generators::NumberGenerator::generate(0, 2);
            switch (num) {
            case 0:
                i = std::make_shared<enemies::BrigandRaider::BrigandRaider>();
                break;
            case 1:
                i = std::make_shared<enemies::BrigandFusilier::BrigandFusilier>();
                break;
            default:
                i = std::make_shared<enemies::CultistAcolyte::CultistAcolyte>();
                break;
            }
        }

        m_enemies = std::shared_ptr<Squad>(new Squad(tmpEnemies, numberOfEnemies));
        m_priority = {};
    }

    std::shared_ptr<Squad> EnemyEncounter::getEnemies() {
        return m_enemies;
    }

    std::vector<std::vector<char>> EnemyEncounter::draw() {
        std::vector<std::vector<char>> to_ret;
        for (auto &i: m_enemies->getEntities()) {
            to_ret.insert(to_ret.end(), i->draw().begin(), i->draw().end());
        }
        return to_ret;
    }

    std::shared_ptr<BattleField> EnemyEncounter::getBattleField() {

    }

    void EnemyEncounter::_enemyMove(Player* player, std::shared_ptr<entity::Entity> entity, int rank, std::shared_ptr<BattleField> battleField) {
        auto skills = entity->getSkills();
        std::vector<std::shared_ptr<skillDesigns::Skill>> availableSkills;
        for (auto i: skills) {
            if (std::find(i->getAvaibleRank().begin(), i->getAvaibleRank().end(), rank) != i->getAvaibleRank().end()) {
                availableSkills.push_back(i);
            }
        }
        int num = generators::NumberGenerator::generate(0, availableSkills.size() - 1);
        auto skillToUse = availableSkills[num];
        int target = generators::NumberGenerator::generate(0, skillToUse->getAvaibleAllyTarget().size() + skillToUse->getAvaibleEnemyTarget().size() - 1);
        if (target < skillToUse->getAvaibleAllyTarget().size()) {
            skillToUse->use(battleField, entity, {player->getSquad()->getEntities()[skillToUse->getAvaibleAllyTarget()[target]]});
        }
        else {
            target -= skillToUse->getAvaibleAllyTarget().size();
            skillToUse->use(battleField, entity, {battleField->getEnemySquad(entity)->getEntities()[skillToUse->getAvaibleEnemyTarget()[target]]});
        }
    }

    bool EnemyEncounter::_checkAlive(std::vector<std::shared_ptr<entity::Entity>> entities) {
        for (auto i: entities) {
            if (i->isAlive()) {
                return true;
            }
        }
        return false;
    }

    void EnemyEncounter::turn(Player* player) {
        std::shared_ptr<BattleField> battleField = std::shared_ptr<BattleField>(new BattleField(player->getSquad(), m_enemies));
        std::vector<std::shared_ptr<entity::Entity>> enemiesEntities = m_enemies->getEntities();
        if (m_priority.empty()) {
            std::vector<std::shared_ptr<entity::Entity>> priority = battleField->getEntities();
            std::sort(priority.begin(), priority.end(), [enemiesEntities] (std::shared_ptr<entity::Entity> a, std::shared_ptr<entity::Entity> b) {
                if (a->get(Characteristic::speed) == b->get(Characteristic::speed)) {
                    return (std::find(enemiesEntities.begin(), enemiesEntities.end(), a) == enemiesEntities.end()) ? true : false;
                }
                return a->get(Characteristic::speed) > b->get(Characteristic::speed);
            });
            for (auto i: priority) {
                if (i->isAlive()) {
                    m_priority.push(i);
                }
            }
        }
        bool heroesAlive = _checkAlive(player->getSquad()->getEntities());
        bool enemiesAlive = _checkAlive(m_enemies->getEntities());
        if (!(heroesAlive && enemiesAlive)) {
            for (auto i: player->getSquad()->getEntities()) {
                endBattleTurnEffects(i);
            }
            for (auto i: enemiesEntities) {
                endBattleTurnEffects(i);
            }
            player->getMap()->getCell(player->getPosition())->freeMoves(player, this);
            return;
        }

        while (!m_priority.empty()) {
            bool heroesAlive = _checkAlive(player->getSquad()->getEntities());
            bool enemiesAlive = _checkAlive(m_enemies->getEntities());
            if (!(heroesAlive && enemiesAlive)) {
                for (auto i: player->getSquad()->getEntities()) {
                    endBattleTurnEffects(i);
                }
                for (auto i: enemiesEntities) {
                    endBattleTurnEffects(i);
                }
                player->getMap()->getCell(player->getPosition())->freeMoves(player, this);
                return;
            }
            std::shared_ptr<entity::Entity> entity = m_priority.back();
            m_priority.pop();
            if (!entity->isAlive()) {
                continue;
            }
            turnEffects(entity);
            if (entity->isTurnable() < 0) {
                continue;
            }
            if (auto markedAsAutoTurn = std::dynamic_pointer_cast<entity::MarkedAsAutoTurn>(entity)) {
                markedAsAutoTurn->autoTurn(std::shared_ptr<Player>(player), battleField, entity, 1);
                player->getMonitor()->draw(player);
                continue;
            }
            if (battleField->areAllies(entity, enemiesEntities[0])) {
                _enemyMove(player, entity, std::distance(enemiesEntities.begin(), std::find(enemiesEntities.begin(), enemiesEntities.end(), entity)) + 1, battleField);
                player->getMonitor()->draw(player);
                continue;
            }
            return;
        }

        turn(player);
    }

}