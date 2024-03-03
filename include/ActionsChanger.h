//
// Created by Амир Кашапов on 28.02.2024.
//

#ifndef CPPDUNGEON_ACTIONSCHANGER_H
#define CPPDUNGEON_ACTIONSCHANGER_H
#include <memory>
#include <set>
#include <functional>
class Player;
class Action;
class ActionsChanger{
protected:
    void addAction(Player*, std::unique_ptr<Action>);
    void removeAction(Player*, std::function<bool (std::set<std::unique_ptr<Action>>::iterator)>);
    void returnToDefault(Player*);
    std::set<std::unique_ptr<Action>>& getActions(Player*);
};
#endif //CPPDUNGEON_ACTIONSCHANGER_H
