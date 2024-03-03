//
// Created by Арсений Бородулин on 03.03.2024.
//
#include "Event.h"

void Event::turn(Player* player, int index) {}

std::vector<std::vector<char>> Event::draw() {
    std::vector<std::vector<char>> sprite;
    sprite.push_back(std::vector<char>{'/',  '-', '-', '-', '-', '-', '\\'});
    sprite.push_back(std::vector<char>{'|',  'E', 'V', 'E', 'N', 'T', '|' });
    sprite.push_back(std::vector<char>{'|',  'E', 'V', 'E', 'N', 'T', '|' });
    sprite.push_back(std::vector<char>{'\\', '-', '-', '-', '-', '-', '/' });
    return sprite;
}





