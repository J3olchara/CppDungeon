//
// Created by Арсений Бородулин on 03.03.2024.
//
#include "ChooseRoomEvent.h"
#include "ChooseNextRoom.h"
#include "Map.h"
#include "Monitor.h"
#include "Player.h"


void ChooseRoomEvent::turn(Player *player, int index) {
    player->getMonitor()->draw();
    player->getMap()->getCell(player->getPosition())->freeMoves(player, index);
//    for (auto *cell : player->getMap()->getNextRooms()) {
//        todo: create ChooseNextRoomConstructor and uncomment below with right parameters
//        this->addAction(player, std::move(std::unique_ptr<ChooseNextRoom>(&ChooseNextRoom(cell))));
//    }
}

std::vector<std::vector<char>> ChooseRoomEvent::draw() {
    std::vector<std::vector<char>> sprite;
    sprite.push_back(std::vector<char>{'/',  '-', '-', '-', '-', '-', '\\'});
    sprite.push_back(std::vector<char>{'|',  'C', 'R', 'O', 'O', 'M', '|' });
    sprite.push_back(std::vector<char>{'|',  'E', 'V', 'E', 'N', 'T', '|' });
    sprite.push_back(std::vector<char>{'\\', '-', '-', '-', '-', '-', '/' });
    return sprite;
}