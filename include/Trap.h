//
// Created by Амир Кашапов on 26.02.2024.
//

#ifndef CPPDUNGEON_TRAP_H
#define CPPDUNGEON_TRAP_H
#include "Event.h"
class Player;
class Trap:public Event{
public:
    void use(Player*, int);
    void dontUse(Player*, int);
};
#endif //CPPDUNGEON_TRAP_H
