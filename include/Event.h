//
// Created by Амир Кашапов on 24.02.2024.
//

#ifndef UNTITLED_EVENT_H
#define UNTITLED_EVENT_H
#include "ActionsChanger.h"
#include <vector>
class Player;
class Action;
class Event:public ActionsChanger{
protected:
public:
    virtual void turn(Player*, int);
    virtual std::vector<std::vector<char>> draw();
    virtual ~Event() = default;
};
#endif //UNTITLED_EVENT_H
