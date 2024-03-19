//
// Created by neb_orion on 09.03.2024.
//

#include "player/Player.h"
#include "navigation/Map.h"
#include "entity/Entity.h"
#include "events/Event.h"
#include "navigation/Cell.h"
#include "monitor/Monitor.h"
#include "actions/ChooseNextRoom.h"
#include "actions/MoveLeft.h"
#include "actions/MoveRight.h"
#include "Squad.h"
#include "events/UsableEvent.h"
#include <iostream>
#include <vector>
#include <ncurses.h>
//Refresh correct pos and size coordinates and add some cooments about resolution and fixed or not sprites 


namespace {
    static const int ENTITY_NUM = 7;
    enum Entity_position {HERO_3, HERO_2, HERO_1, CHEST, ENEMY_1, ENEMY_2, ENEMY_3};
    
    enum Colors : short {CELL_COLOR = COLOR_PAIR(1), ROOM_COLOR = COLOR_PAIR(2), 
    CUR_ROOM_COLOR = COLOR_PAIR(3), NEXT_ROOM_COLOR = COLOR_PAIR(4), 
    INTERFACE_COLOR = COLOR_PAIR(5), ITEM_COLOR = COLOR_PAIR(6), TARGETED_ROOM_COLOR = COLOR_PAIR(7)};
    enum Map_symbols : char {CELL = '"', ROOM = '0', TARGET_ROOM = '#'};
}


Monitor::GameWindow::GameWindow(const size_t& y_size, const size_t& x_size, const size_t& pos_y, const size_t& pos_x)
: m_y_size(y_size)
, m_x_size(x_size) {
    m_current_window = newwin(m_y_size, m_x_size, pos_y, pos_x);
    //DEBUG: for visual size of areas, will remove this when drawing ready sprites
    wborder(m_current_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(m_current_window);
}

Monitor::GameWindow::GameWindow()
: m_y_size(0)
, m_x_size(0)
, m_current_window(nullptr) {}


void Monitor::GameWindow::set_atr(size_t row, size_t col, size_t num, attr_t atr, short color) {
    mvwchgat(m_current_window, row, col, num, atr, color, NULL);
    wrefresh(m_current_window);
}

void Monitor::GameWindow::clear_atr(size_t row, size_t col, int num) {
    if (num == -1) {
        wattrset(m_current_window, A_NORMAL);
        wcolor_set(m_current_window, 0, NULL);
    } else {
        this->set_atr(row, col, num);
    }
}

void Monitor::GameWindow::clean() {
    wclear(m_current_window);
    //DEBUG: for visual size of areas, will remove this when drawing ready sprites
    wborder(m_current_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(m_current_window);
}
//TODO: add size checkers
//draw_sprite draws with only either text formating attribute or pair of colors (text, font) 
void Monitor::GameWindow::draw_sprite(const size_t& pos_y, const size_t& pos_x,
                        const std::vector<std::vector<char>>& sprite, bool skip_spaces, int attribute) {
    wattron(m_current_window, attribute);
    size_t current_y = pos_y;
    size_t current_x = pos_x;
    for (const std::vector<char>& current_string : sprite) {
        //Ncurses can only draw char or const char*,  so need to write by symbol
        wmove(m_current_window, current_y, current_x);
        for (char symbol : current_string) {
            if (symbol == ' ' && skip_spaces) {
                wmove(m_current_window, current_y, current_x + 1);
            } else {
                waddch(m_current_window, symbol);
            }
            current_x++;
        }
        current_y++;
        current_x = pos_x;
    }
    wattroff(m_current_window, attribute);
    wrefresh(m_current_window);
}


void Monitor::GameWindow::draw_text(const size_t& pos_y, const size_t& pos_x,
                        const std::string& text, bool skip_spaces, int attribute) {
    draw_sprite(pos_y, pos_x, {1, std::vector<char>(text.begin(), text.end())}, skip_spaces, attribute);
}

size_t Monitor::GameWindow::get_x() const{
    return m_x_size;
}

size_t Monitor::GameWindow::get_y() const{
    return m_y_size;
}

Monitor::GameWindow::GameWindow (const Monitor::GameWindow& other) { 
    m_current_window = other.m_current_window;
    m_x_size = other.m_x_size;
    m_y_size = other.m_y_size;
}

Monitor::InterfaceColumnWindow::InterfaceColumnWindow(const size_t& y_size, const size_t& x_size, const size_t& pos_y, const size_t& pos_x)
: GameWindow(y_size, x_size, pos_y, pos_x) {
    int num_of_col = 5;
    int col_size = 8;
    int block_size  = this->get_x() / (num_of_col * col_size + (num_of_col + 1));
    int number_of_blocks = this->get_x() / block_size;
    for (int i = 0; i < num_of_col; i++) {
        m_columns.push_back(GameWindow(this->get_y(), col_size * block_size, pos_y, pos_x + block_size * (1 + (col_size + 1) * i)));
    }
}

Monitor::InterfaceColumnWindow::InterfaceColumnWindow(const InterfaceColumnWindow& other) {
    
    m_columns = other.m_columns;
    m_current_window = other.m_current_window;
    m_key_binds = other.m_key_binds;
    m_x_size = other.m_x_size;
    m_y_size = other.m_y_size;
    m_first_unbind = other.m_first_unbind;
};

int Monitor::InterfaceColumnWindow::find_bind_key(std::shared_ptr<actions::Action> action) {
    for (auto& i : m_key_binds) {
        if (i.second == action) {
            return i.first;
        }
    }
    return 0;
}

 std::shared_ptr<actions::Action> Monitor::InterfaceColumnWindow::find_action(int key) {
    if (m_key_binds.find(key) == m_key_binds.end()) {
        return nullptr;
    }
    return m_key_binds[key];
 }
//TODO: Add rebase or listing if the actions have a too much space
void Monitor::InterfaceColumnWindow::draw_interface(std::set<std::shared_ptr<actions::Action>> available_actions, bool adaptive) {
    
    m_first_unbind = '0';
    size_t cur_y = 0;
    size_t cur_column = 0;
    for (auto& i : m_columns) {
        i.clean();
    }
    for (auto& i : available_actions) {
        if (cur_y >= m_columns[cur_column].get_y() - 1) {
            cur_y = 0;
            cur_column++;
        }
        if (adaptive && this->find_bind_key(i) == 0) {
            m_key_binds[m_first_unbind] = i;
            m_first_unbind++;
        }
        m_columns[cur_column].draw_text(cur_y, 0, std::string(1, static_cast<char>(this->find_bind_key(i))) + " : " + i->getName(), false, INTERFACE_COLOR);
        cur_y += 2;
    }
};

Monitor::InterfaceColumnWindow::InterfaceColumnWindow()
:m_columns({})
, m_key_binds({}) {}

void Monitor::InterfaceColumnWindow::get_binds(Player* player) {
    if(player == nullptr){
        throw std::logic_error("Player undefined");
    }
    for(auto& action:player->getActions()){
        if(auto chooseNextRoom = std::dynamic_pointer_cast<actions::ChooseNextRoom>(action)){
            switch (player->getMap()->getDirecrion(player->getPosition(), chooseNextRoom->getPostion())) {
                case Map::direction::up:
                    m_key_binds['i'] = chooseNextRoom;
                    break;
                case Map::direction::down:
                    m_key_binds['k'] = chooseNextRoom;
                    break;
                case Map::direction::left:
                    m_key_binds['j'] = chooseNextRoom;
                    break;
                case Map::direction::right:
                    m_key_binds['l'] = chooseNextRoom;
                    break;
            }
        }
        else if(auto moveLeft = std::dynamic_pointer_cast<actions::MoveLeft>(action)){
            m_key_binds['a'] = moveLeft;
        }
        else if(auto moveRight = std::dynamic_pointer_cast<actions::MoveRight>(action)){
            m_key_binds['d'] = moveRight;
        }
    }
}


Monitor::Monitor() {
    int row, col;
    getmaxyx(stdscr, row, col);
    
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    init_pair(4, COLOR_GREEN, COLOR_GREEN);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_WHITE);
    //        |--------------------|
    // 2 / 3  |  Battle            |
    //        |                    |
    //        |                    |
    //        |----------| ------- |
    // 1 / 3  | Inventory|      Map|
    //        |          |         |
    m_background_display = GameWindow ( 2 * row / 3, col, 0, 0);
    m_inventory_display.push_back(GameWindow (8 * row / 9, col / 4, 7 * row / 9 + 1, 0));
    m_inventory_display.push_back(GameWindow (8 * row / 9, col / 4, 7 * row / 9 + 1, col / 4 + 1));
    m_map_display = GameWindow (row / 3, col / 2, row * 2 / 3 + 1, col / 2 + 1);
    m_user_actions_display = InterfaceColumnWindow(row / 9, col / 2, 2 * row / 3 + 1, 0);
    //Calculating x_distance and size
    const int heroes_blocks = 4;
    const int edge_blocks = 1;
    const int space_blocks = 1;
    int block_dictance = col / ((ENTITY_NUM - 1) * space_blocks + ENTITY_NUM * heroes_blocks + edge_blocks * 2);
    // blocks between heroes, blocks on hero, board edge blocks
    int left_dictance = col % ((ENTITY_NUM - 1) * space_blocks + ENTITY_NUM * heroes_blocks + edge_blocks * 2);



    for (int i = 0; i < ENTITY_NUM; i++) {
        m_entity_window.push_back(GameWindow(8 * m_background_display.get_y() / 10,
                                            heroes_blocks * block_dictance, m_background_display.get_y() / 10,
                                            block_dictance * ((heroes_blocks + space_blocks) * i + edge_blocks) + left_dictance / 2));
    }
}

void Monitor::abs_coordinates_to_relative(int& row, int& col, const GameWindow& cur_window, Position center) {
    int row_realtive_center = cur_window.get_y() / 2 - (cur_window.get_y() + 1) % 2;
    int col_realtive_center = cur_window.get_x() / 2 - (cur_window.get_x() + 1) % 2;
    int row_difference = center.getLine() - row;
    int col_difference = center.getColumn() - col;
    row = row_realtive_center - row_difference;
    col = col_realtive_center - col_difference;
}


//TODO: Change start postion of all sprites
void Monitor::draw(Player* current_player) {
    //Heroes
    int draw_position = static_cast<int>(Entity_position::HERO_1);
    if(current_player->getSquad() == nullptr){
        throw std::logic_error("Squad is not initialized");
    }
    for (const std::shared_ptr<entity::Entity>& i : current_player->getSquad()->getEntities()) {
        m_entity_window[draw_position].clean();
        if (i != nullptr) {
            m_entity_window[draw_position].draw_sprite(0, 0, i->draw());
        }
        draw_position--;
    }
    std::vector<std::vector<char>> drawing_map = current_player->getMap()->draw(current_player->getPosition(),
                                                                                m_map_display.get_y() - 2, 
                                                                                m_map_display.get_x() - 2);
    
    for (auto& row : drawing_map) {
        for (auto& symbol : row) {
            if (symbol != 1) {
                symbol = ' ';
            } else {
                symbol = Map_symbols::ROOM;
            }
        }
    }
    m_map_display.clean();
    m_map_display.draw_sprite(1, 1, drawing_map, true, Colors::ROOM_COLOR);
    drawing_map = current_player->getMap()->draw(current_player->getPosition(),
                                                                                m_map_display.get_y() - 2, 
                                                                                m_map_display.get_x() - 2);
    
    for (auto& row : drawing_map) {
        for (auto& symbol : row) {
            if (symbol != 2) {
                symbol = ' ';
            } else {
                symbol = Map_symbols::CELL;
            }
        }
    }
    m_map_display.draw_sprite(1, 1, drawing_map, true, Colors::CELL_COLOR);
    int center_x = current_player->getPosition().getColumn();
    int center_y = current_player->getPosition().getLine();
    abs_coordinates_to_relative(center_y, center_x, m_map_display, current_player->getPosition());
    m_map_display.set_atr(center_y, center_x, 1, A_ITALIC, 3);

    for (Position i : current_player->getMap()->getNextRooms(current_player->getPosition())) {
        int cur_x = i.getColumn();
        int cur_y = i.getLine();
        abs_coordinates_to_relative(cur_y, cur_x, m_map_display, current_player->getPosition());
        if (cur_x != 0 && cur_x != m_map_display.get_x() && cur_y != 0 && cur_y != m_map_display.get_y()) {
            m_map_display.set_atr(cur_y, cur_x, 1, A_BLINK, 4); 
        }
    }
    //Current room drawing would be here
    Position next = current_player->getMap()->getNextRoom(current_player->getPosition());
    if (!(next == current_player->getPosition())) {
        int next_x = next.getColumn();
        int next_y = next.getLine();
        abs_coordinates_to_relative(next_y, next_x, m_map_display, current_player->getPosition());
        m_map_display.draw_text(next_y, next_x, std::string(1, Map_symbols::TARGET_ROOM), false, TARGETED_ROOM_COLOR);
    }
    
    m_user_actions_display.m_key_binds = {};
    m_user_actions_display.get_binds(current_player);
    m_user_actions_display.draw_interface(current_player->getActions());

    
    for(auto event:current_player->getMap()->getCell(current_player->getPosition())->getEvents()){
        if(auto usableEvent = std::dynamic_pointer_cast<events::UsableEvent>(event)){
            m_entity_window[Entity_position::CHEST].draw_sprite(0, 0, usableEvent->draw()); 
        }
    }

    //inventory 
    /*int cur_y = 0;
    int cur_column = 0;
    for (auto item : current_player->getInventory().getItems()) {
        if (m_inventory_display[cur_column].get_y() - 2 <= cur_y) {
            cur_column++;
            cur_y = 0;
        }
        m_inventory_display[cur_column].draw_text(cur_y, 0, item->draw(), false, Colors::ITEM_COLOR);
        cur_y += 2;
    }*/
    

}



void Monitor::keyEvent(int key, Player* player) {
    if (m_user_actions_display.find_action(key) != nullptr && 
    player->getActions().find(m_user_actions_display.find_action(key)) != player->getActions().end()) {
        m_user_actions_display.find_action(key)->act(player);
    }
}



void Monitor::keyEvent(Player* player) {
    char pressed_key = getch();
    keyEvent(pressed_key, player);
}

