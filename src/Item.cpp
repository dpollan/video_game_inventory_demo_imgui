#include <iostream>
#include <string>
#include <vector>
#include "Item.h"


using std::cout; 
using std::endl;
using std::vector;
using std::string;

// Default Constructor - generates a dummy Item with id and type set to -1
Item::Item() 
{
    std::vector<equipped_mods> placeholder_equipped_mods;
    id = -1; name = "STUB"; type = -1; equipped_modifiers = placeholder_equipped_mods; is_equipped = false;
}


// Constructor
Item::Item(int item_id, std::string item_name, int item_type, std::vector<equipped_mods> v_mods, bool item_is_equipped) {
    id = item_id; name = item_name; type = item_type; equipped_modifiers = v_mods; is_equipped = false;
}

// Copy/Move Constructor
Item::Item(const Item &item) {
    id = item.id; name = item.name; type = item.type; equipped_modifiers = item.equipped_modifiers; is_equipped = item.is_equipped;
}

// Destructor - Primarily for debugging, since smart pointers and vectors clean themselves up
Item::~Item() {
    //std::cout<<"DEBUG:  destructor called for "<<this->get_name()<<std::endl;
}


void Item::item_equip(Character* owner){
    for (equipped_mods mod : this->equipped_modifiers) 
    {
        owner->update_stats(mod.mod_category, mod.mod_value, true);
        is_equipped = true;
    }
}
void Item::item_unequip(Character* owner){
    for (equipped_mods mod : this->equipped_modifiers) 
    {
        owner->update_stats(mod.mod_category, mod.mod_value, false);
        is_equipped = false;
    }
}

// Getters
int Item::get_id()
{
    return id;
}

int Item::get_type()
{
    return type;
}
std::string Item::get_type(int i) 
{
    switch (i) {
        case 0: 
            return "Body Armor";
        case 1:
            return "Helmet";
        case 2:
            return "Gloves";
        case 3:
            return "Boots";
        case 4:
            return "Weapon";
        default:
            return "No Idea";
    }
}

std::vector<equipped_mods> Item::get_mods() 
{
    return equipped_modifiers;
}

string Item::get_name()
{
    return name;
}
