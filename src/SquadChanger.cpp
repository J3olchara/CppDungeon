//
// Created by Арсений Бородулин on 10.03.2024.
//
#include "SquadChanger.h"
#include "player/Squad.h"

void SquadChanger::move(const std::shared_ptr<Squad>& squad, int index1, int index2) {
    if (index1 < 0 || index1 >= squad->m_squad.size()) { throw std::invalid_argument("index is out of range"); }
    if (index2 >= squad->m_squad.size())  { index2 = static_cast<int>(squad->m_squad.size()) - 1;}
    if (index2 < 0)                     { index2 = 0; }
    if (squad->m_squad[index2] == nullptr) {
        squad->m_squad[index2] = squad->m_squad[index1];
        squad->m_squad[index1] = nullptr;
        return;
    }
    if (index1 == index2) { return; }

    std::shared_ptr<Entity> tmp = squad->m_squad[index2];
    squad->m_squad[index2] = nullptr;
    int const move_to = index1 < index2 ? -1 : 1;
    int ptr = index2 + move_to;
    while (tmp != nullptr && ptr - move_to != index1) { // index(tmp) < ptr, just chainswap
        std::swap(tmp, squad->m_squad[ptr]);
        ptr += move_to;
    }
    if (tmp != nullptr) squad->m_squad[index2] = tmp;
}


void SquadChanger::move(const std::shared_ptr<Squad>& squad, const std::shared_ptr<Entity>& entity, int index2) {
    for (int i = 0; i < squad->m_squad.size(); ++i) {
        if (squad->m_squad[i] == entity) {
            return SquadChanger::move(squad, i, index2);
        }
    }
    throw std::invalid_argument("entity is not in m_squad");
}
void SquadChanger::relativeMove(const std::shared_ptr<Squad>& squad, int index, int offset) {
    SquadChanger::move(squad, index, offset);
}
void SquadChanger::relativeMove(const std::shared_ptr<Squad>& squad, const std::shared_ptr<Entity>& entity, int offset) {
    SquadChanger::move(squad, entity, offset);
}
void SquadChanger::remove(const std::shared_ptr<Squad>& squad, int index) {
    if (index < 0 || index >= squad->m_squad.size()) { throw std::invalid_argument("index is out of range"); }
    squad->m_squad[index] = nullptr;
}
void SquadChanger::remove(const std::shared_ptr<Squad>& squad, const std::shared_ptr<Entity>& entity) {
    for (int i = 0; i < squad->m_squad.size(); ++i) {
        if (squad->m_squad[i] == entity) {
            squad->m_squad[i] = nullptr;
            return;
        }
    }
    throw std::invalid_argument("entity is not in m_squad");
}
void SquadChanger::add(const std::shared_ptr<Squad>& squad, std::shared_ptr<Entity>entity, int index) {
    if (index < 0 || index >= squad->m_squad.size()) { throw std::invalid_argument("index is out of range"); }
    if (squad->m_squad[index] == nullptr) {
        squad->m_squad[index] = std::move(entity);
        return;
    }
    throw std::invalid_argument("index is not empty");
}
void SquadChanger::add(const std::shared_ptr<Squad>& squad, std::shared_ptr<Entity> entity) {
    for (int i = 0; i < squad->m_squad.size(); ++i) {
        if (squad->m_squad[i] != nullptr) {
            squad->m_squad[i] = std::move(entity);
            return;
        }
    }
    throw std::length_error("m_squad is full!");
}