#include <iostream>
#include <string>
#include <vector>
#include "Character.h"


using std::cout; 
using std::endl;
using std::string;
using std::vector;

Character::Character(std::string character_name, double str, double agl, double spd, double inl, double lck, double maxHP, double maxMN) 
{
    name = character_name;
    strength = str; agility = agl; speed = spd; intelligence = inl; luck = lck;
    current_health = maxHP; current_mana = maxMN; max_health = maxHP; max_mana = maxMN;
}

void Character::print_all()
{
    cout << "\nCharacter: " << this->name << endl;
    cout << "_______stats_______" << endl;
    cout << "Strength: " << this->strength << endl;
    cout << "Agility: " << this->agility << endl;
    cout << "Speed: " << this->speed << endl;
    cout << "Intelligence: " << this->intelligence << endl;
    cout << "Luck: " << this->luck << endl;
    cout << "Health: " << this->current_health << "/" << this->max_health << endl;
    cout << "Mana: " << this->current_mana << "/" << this->max_mana << endl;
}
    
void Character::print_equipped()
{
    cout << "\nCharacter " << this->name << " is equipped with the following:" << endl;

    // Chest
    if (this->equipped_items[0]) 
    {
        cout << "Chest Armor: " << this->equipped_items[0]->get_name() << endl;
    } else {
        cout << "Chest Armor: None" << endl;
    }

    // head
    if (this->equipped_items[1]) 
    {
        cout << "Head Gear: " << this->equipped_items[1]->get_name() << endl;
    } else {
        cout << "Head Gear: None" << endl;
    }

    // Gloves
    if (this->equipped_items[2]) 
    {
        cout << "Gloves: " << this->equipped_items[2]->get_name() << endl;
    } else {
        cout << "Gloves: None" << endl;
    }

    // Boots
    if (this->equipped_items[3]) 
    {
        cout << "Boots: " << this->equipped_items[3]->get_name() << endl;
    } else {
        cout << "Boots: None" << endl;
    }

    // Weapon
    if (this->equipped_items[4]) 
    {
        cout << "Weapon: " << this->equipped_items[0]->get_name() << endl;
    } else {
        cout << "Weapon: None" << endl;
    }
}

void Character::print_inventory(){
    // Iterate through all item's in the player's bag
    if (bag.size() == 0) 
    {
        std::cout << "\nThe player's inventory is empty.\n";
    } else {
        cout << "\nThe Player's inventory contains the following: " << endl;
        for (Item &item : bag) {
            cout << "[" << item.get_name() << "] ";
        }
        cout << "\nBag contains " << bag.size() << " of 30 items" << endl; 
    }
}

Item Character::equip_item(Item* item_to_equip)
{
    // Get item to equip id and type
    int item_id = item_to_equip->get_id();
    int item_type = item_to_equip->get_type();

    // Get the index of the item to equip in the bag vector using id
    int bag_slot = -1;
    int compare_id;

    // This works since player's bag is small (only 30 Items). If the bag scales much larger I'd probably
    // need to implement some kind of a hash table because looping till we find it gets expensive
    for (int i = 0; i < bag.size(); i++) 
    {
        compare_id = bag[i].get_id();
        if (item_id == compare_id) {
            bag_slot = i;
            break;               
        }
    }

    // If no item of this type is currently in the equipped slot, equip item and remove from bag
    if (equipped_items[item_type] == 0) 
    {
        equipped_items[item_type] = item_to_equip;
        bag.erase(bag.begin() + bag_slot);
        equipped_items[item_type]->item_equip(this);
        return *equipped_items[item_type];
    } else {
        // Otherwise swap the current equipped item with the new item
        bag[bag_slot] = *equipped_items[item_type];
        bag[bag_slot].item_unequip(this);
        equipped_items[item_type] = item_to_equip;
        equipped_items[item_type]->item_equip(this);
        return *equipped_items[item_type];
    }
}

void Character::pick_up_item(Item* item_to_place_in_bag)
{
    if (bag.size() < 30) {
        bag.push_back(*item_to_place_in_bag);
    } else {
        std::cout << "There is no room in your inventory to pick up " << item_to_place_in_bag->get_name() << ", drop something to make room." << std::endl;
    }
}

// This method literally just takes the string and converts it to the corresponding enum 
stats Character::convert_to_enum(string const& stat) 
{
    if (stat == "str") return str;
    if (stat == "agl") return agl;
    if (stat == "spd") return spd;
    if (stat == "inl") return inl;
    if (stat == "lck") return lck;

    if (stat == "maxHP") return maxHP;
    if (stat == "maxMN") return maxMN;
}

// Since equipped Items kept going out of scope in ImGui list box I needed a way to keep them around after list completes
// By making static slots for each equip slot and passing a reference to it they persist undil replaced
// It's not an ideal solution but it is functional
void Character::populate_equipped_with_static_refs(Item *armor, Item *helmet, Item *gloves, Item *boots, Item*weapon) 
{
    // If the stack array of Item pointers still has "STUB" it has placholder Item
    // This is effectively the same as not having an item equipped in the specified slot
    // Here we set the stack array to either NULL or the pointer parameter
    equipped_items[0] = armor->get_name() == "STUB" ? 0 : armor;
    equipped_items[1] = helmet->get_name() == "STUB" ? 0 : helmet;
    equipped_items[2] = gloves->get_name() == "STUB" ? 0 : gloves;
    equipped_items[3] = boots->get_name() == "STUB" ? 0 : boots;
    equipped_items[4] = weapon->get_name() == "STUB" ? 0 : weapon;
}

void Character::set_equipped_item(Item * item) 
{
    int item_type = item->get_type();
    Item * newItem = new Item(); // <- copy constructor creates new Item
    newItem = item;
    equipped_items[item_type] = newItem;
    this->print_equipped();
}

// Getters

string Character::get_name() 
{
    return this->name;
}

double Character::get_strength() 
{
    return this->strength;
}

double Character::get_agility() 
{
    return this->agility;
}
double Character::get_intelligence() 
{
    return this->intelligence;
}
double Character::get_speed() 
{
    return this->speed;
}
double Character::get_luck() 
{
    return this->luck;
}

double Character::get_max_hp() 
{
    return this->max_health;
}
double Character::get_current_hp() 
{
    return this->current_health;
}
double Character::get_max_mana() 
{
    return this->max_mana;
}
double Character::get_current_mana() 
{
    return this->current_mana;
}

std::vector<Item> Character::get_bag() 
{
    return this->bag;
}

Item* Character::get_equipped(int slot) 
{
    return equipped_items[slot];
}

// End Getters
    
void Character::update_stats(std::string stat, int value, bool added){

    // If the added parameter is not true, multiply by -1 to negate affect as the item was removed
    if (!added) {
        value *= -1;
    }

    switch (this->convert_to_enum(stat)) {
        case str: 
            strength+=value;
            break;
        case agl: 
            agility+=value;
            break;
        case spd: 
            speed+=value;
            break;
        case inl: 
            intelligence+=value;
            break;
        case lck: 
            luck+=value;
            break;
        case maxHP: 
            max_health+=value;
            current_health+=value;
            break;
        case maxMN: 
            max_mana+=value;
            current_mana+=value;
            break;
    }    
}
