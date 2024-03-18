#include "monitor/Monitor.h"
#include "navigation/Map.h"
#include "player/Player.h"
#include "events/EnemyEncounter.h"
#include "enemies/BrigandRaider/BrigandRaider.h"
#include "entity/Entity.h"
#include "entity/MarkedAsAutoTurn.h"

#include "Squad.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>

bool check(std::vector<std::shared_ptr<entity::Entity>> entities) {
    for (auto i: entities) {
        if (i && i->isAlive()) {
            return true;
        }
    }
    return false;
}

int main() {

    auto enemyEncounter = std::shared_ptr<events::EnemyEncounter>(new events::EnemyEncounter());
    auto monitor = std::shared_ptr<Monitor>(new Monitor(enemyEncounter));
    auto map = std::shared_ptr<Map>(new Map(12));
    auto squad = std::shared_ptr<Squad>(new Squad({std::make_shared<enemies::BrigandRaider::BrigandRaider>()}));
    auto player = std::shared_ptr<Player>(new Player(map, monitor, squad));

    while (check(player->getSquad()->getEntities()) && check(enemyEncounter->getEnemies()->getEntities())) {
        enemyEncounter->turn(player.get());
    }

    return 0;
}