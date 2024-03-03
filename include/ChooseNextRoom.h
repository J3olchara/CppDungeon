//
// Created by Амир Кашапов on 26.02.2024.
//

#ifndef CPPDUNGEON_CHOOSENEXTROOM_H
#define CPPDUNGEON_CHOOSENEXTROOM_H
#include "Action.h"
#include "PositionChanger.h"
class ChooseNextRoom:public Action, public PositionChanger{
public:
    void act(Player*) override; //Ask arguments from who realizes Map
};
#endif //CPPDUNGEON_CHOOSENEXTROOM_H
