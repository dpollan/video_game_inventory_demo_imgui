#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <string>
#include <vector>
#include "Item.h"

class Item;
struct equipped_mods;

enum stats {str, agl, spd, inl, lck, maxHP, maxMN};

class Character {
private:

    std::string name;

    double strength;
    double agility;
    double speed;
    double intelligence;
    double luck;

    double current_health;
    double current_mana;

    double max_health;
    double max_mana;

    std::vector<Item> bag;
    Item * equipped_items[5] = {0,0,0,0,0}; // maybe change this to vector and manage size yourself

public:
    Character(std::string character_name, double str, double agl, double spd, double inl, double lck, double maxHP, double maxMN);
    Character(const Character &character);
    void print_all();
    void print_equipped();
    void print_inventory();
    Item equip_item(Item* item_to_equip);
    void pick_up_item(Item* item_to_place_in_bag);
    void update_stats(std::string stat, int value, bool added);
    stats convert_to_enum(std::string const& stat);
    void populate_equipped_with_static_refs(Item * armor, Item * helmet, Item * gloves, Item * boots, Item * weapon);
    void set_equipped_item(Item * item);

    //Getters
    std::string get_name();
    double get_strength();
    double get_agility();
    double get_speed();
    double get_intelligence();
    double get_luck();

    double get_max_hp();
    double get_current_hp();
    double get_max_mana();
    double get_current_mana();

    std::vector<Item> get_bag();
    Item * get_equipped(int slot);
};

#endif
