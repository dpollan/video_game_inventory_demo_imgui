#ifndef ITEM_H
#define ITEM_H

#include<iostream>
#include<vector>
#include<string>
#include "Character.h"

class Character;

struct equipped_mods {std::string mod_category; int mod_value;};
// Currently equipped mods must be created first and passed as a Vector to Item Constructor
// TODO: figure out a better way to store and handle mods 

class Item {
private:
    int id;
    std::string name;
    int type;
    std::vector<equipped_mods> equipped_modifiers;
    bool is_equipped;
public:
    Item();
    Item(int item_id, std::string item_name, int item_type, std::vector<equipped_mods> v_mods, bool item_is_equipped);
    Item(const Item &item);
    ~Item();
    void item_equip(Character* owner);
    void item_unequip(Character* owner);
    // Getters
    int get_id();
    int get_type();
    std::string get_type(int i);
    std::vector<equipped_mods> get_mods();
    std::string get_name();
};

#endif

