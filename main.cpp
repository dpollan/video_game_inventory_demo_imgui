#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "opengl_shader.h"
#include "file_manager.h"
#include <stdio.h>
#include <iostream>
#include <crtdbg.h>
#include <vector>
#include <memory>
#include "src/Item.h"
#include "src/Character.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const bool DEBUG_ON = false;

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
	// *******************  BEGIN GLFW and ImGui Setup stuff *******************

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

		// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char *glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	bool err = glewInit() != GLEW_OK;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ************ END GLFW and ImGui Setup Stuff ******************


	// Set up static memory addresses to store Equipped items so things don't go out of scope in ImGui loop
	// Since all itmes are smart pointers they get destroyed when going out of scope, this kees a reference to them aroud
	// TODO: Figure out a better way to handle this.
	static Item armor_slot;
	static Item helmet_slot;
	static Item glove_slot;
	static Item boot_slot;
	static Item weapon_slot;

	// Also keep a statc memory version of the player inventory around 
	// this is so we can repeatedly poll the Player inventory to populate list box for updated state
	static std::vector<Item> inventory;

	// *****  BEGIN Generate some test data ******* 
	std::cout<<"Creating Player and some sample equipment.\n\n\n";
    std::unique_ptr<Character> Player(new Character("Default Player", 50, 50, 50, 50, 50, 100, 100));

	// Generate item modifiers -- TODO: Clean this up and create a tool for making new Items automatically
    equipped_mods lh_spd = {"spd",5}; 
    equipped_mods lh_agl = {"agl",20};
    equipped_mods ih_str = {"str",15}; 
    equipped_mods ih_agl = {"agl",5};
	equipped_mods la_maxHP = {"maxHP", 85};
	equipped_mods sa_lck = {"lck", 20};
	equipped_mods sa_maxHP = {"maxHP", 200};
	equipped_mods lg_spd = {"spd", 10};
	equipped_mods lg_lck {"lck", -5};
	equipped_mods sb_str = {"str", 10};
	equipped_mods sb_lck = {"lck", 20};
	equipped_mods lb_spd = {"spd", 50};
	equipped_mods lb_maxHP = {"maxHP", 20};
	equipped_mods ms_max_mp = {"maxMN", 300};
	equipped_mods ms_inl = {"inl", 50};
	equipped_mods ls_str = {"str", 30};
	equipped_mods ls_spd = {"spd", 10};

    std::vector<equipped_mods> lh_mods;
    std::vector<equipped_mods> ih_mods;
	std::vector<equipped_mods> la_mods;
	std::vector<equipped_mods> sa_mods;
	std::vector<equipped_mods> lg_mods;
	std::vector<equipped_mods> sb_mods;
	std::vector<equipped_mods> ms_mods;
	std::vector<equipped_mods> ls_mods;
	std::vector<equipped_mods> lb_mods;

    lh_mods.push_back(lh_spd); lh_mods.push_back(lh_agl);
    ih_mods.push_back(ih_str); ih_mods.push_back(ih_agl);
	la_mods.push_back(la_maxHP);
	lb_mods.push_back(lb_spd);lb_mods.push_back(lb_maxHP);
	sa_mods.push_back(sa_maxHP); sa_mods.push_back(sa_lck);
	lg_mods.push_back(lg_spd); lg_mods.push_back(lg_lck);
	sb_mods.push_back(sb_str); sb_mods.push_back(sb_lck);
	ms_mods.push_back(ms_max_mp); ms_mods.push_back(ms_inl);
	ls_mods.push_back(ls_spd);ls_mods.push_back(ls_str);

	// Now generate and populate mods for each item on the ground by the player
    std::unique_ptr<Item> leather_helmet(new Item(1, "Leather Helmet", 1, lh_mods, false));
    std::unique_ptr<Item> iron_helmet(new Item(2, "Iron Helmet", 1, ih_mods, false));
	std::unique_ptr<Item> leather_armor(new Item(3, "Leather Armor", 0, la_mods, false));
	std::unique_ptr<Item> steel_armor(new Item(4, "Steel Armor", 0, sa_mods, false));
	std::unique_ptr<Item> leather_gloves(new Item(5, "Leather Gloves", 2, lg_mods, false));
	std::unique_ptr<Item> leather_boots(new Item(6, "Leather Boots", 3, lb_mods, false));
	std::unique_ptr<Item> steel_boots(new Item(7, "Steel Boots", 3, sb_mods, false));
	std::unique_ptr<Item> magic_staff(new Item(8, "Magic Staff", 4, ms_mods, false));
	std::unique_ptr<Item> long_sword(new Item(9, "Long Sword", 4, ls_mods, false));
	// *****  END Generate test data ******* 




	// Player pick up all the items and place them in Player bag
	Player->pick_up_item(leather_helmet.get());
	Player->pick_up_item(leather_armor.get());
	Player->pick_up_item(leather_gloves.get());
	Player->pick_up_item(leather_boots.get());
	Player->pick_up_item(iron_helmet.get());
	Player->pick_up_item(steel_armor.get());
	Player->pick_up_item(steel_boots.get());
	Player->pick_up_item(magic_staff.get());
	Player->pick_up_item(long_sword.get());



	// Populate static memory Inventory with initial state of Player bag
	inventory = Player->get_bag();


	// *******  BEGIN main ImGui loop **********
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// -------------------  BEGIN UI CHARACTER Window -------------------
		ImGui::Begin("Character");

		// Display the Name and current stats for the character

		// Name
		ImGui::Text(Player->get_name().data()); // Use .data() since we get a String back and need a char* for ImGui

		// Stats 
		// All Double values get cast to ints for better presentation in the stats UI
		ImGui::Text("Strength:     ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_strength()).data());

		ImGui::Text("Agility:      ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_agility()).data());

		ImGui::Text("Speed:        ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_speed()).data());

		ImGui::Text("Intelligence: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_intelligence()).data());


		ImGui::Text("Luck:         ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_luck()).data());

		ImGui::Text("Health: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_current_hp()).data());
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_max_hp()).data());

		ImGui::Text("Mana: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_current_mana()).data());
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		ImGui::Text(std::to_string( (int) Player->get_max_mana()).data());

		ImGui::End();
		// ------------------------ End UI CHARACTER window -------------------------


		// -----------------------  BEGIN UI INVENTORY window ----------------------
		ImGui::Begin("Inventory:");
		
		// Here we basically need a array of pointers to chars, NOT a Vector of strings.  This caused me quite a headache
		// My current naive solution: initialize static char* array with pointers to "" empty strings
		// Then populate the static slots by iterating through Player.bag Vector and calling Item::get_name() 
		// This seems overly complex, error prone and hacky but it's my first attempt MVP solution
		// TODO: Find a better way!

		static char* inventory_names[30] {"","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""};
		static int item_count = inventory.size();
		static int current_item = 0;

		char* entry;

		for (int i = 0; i < 30; i++) 
		{
			if (i < inventory.size()) 
			{
				char buffer[50];  // <-- assuming no Item name is longer than 50 characters, prob should make this a Item Const
				strcpy_s(buffer, inventory[i].get_name().c_str()); 
				entry = new char[strlen(buffer + 1)];  // <-- I think I foud the Memory leak maybe
				// Dreaded conversion from string to char array
				for (int j = 0; j < strlen(buffer); j++)
				{
					entry[j] = buffer[j];
					if (j == strlen(buffer + 1))
					{
						entry[j+1] = '\0';
					}
				}
				inventory_names[i] = entry;
				continue;
			}
			break;
		}
		
		delete entry;

		ImGui::ListBox("Inventory", &current_item, inventory_names, IM_ARRAYSIZE(inventory_names), 10);
		ImGui::End();
		// ----------------------  End UI INVENTORY window ------------------------

		// ---------------------- Begin DETAILS window -------------------------
		ImGui::Begin("Item Details");

		// Display the name and type of item along with a description of any stats modifiers that occur
		// when equipping this item.  

		// This window also contains a button allowing user to equip items in their inventory 

		if (current_item < item_count) 
		{
			ImGui::Text("Item: ");
			ImGui::SameLine();
			ImGui::Text(inventory[current_item].get_name().data());
			ImGui::Text("Type: ");
			ImGui::SameLine();
			int item_type = inventory[current_item].get_type();
			ImGui::Text(inventory[current_item].get_type(item_type).data());
			ImGui::Text("Modifications:");
			std::vector<equipped_mods> item_mods = inventory[current_item].get_mods();
			for (auto it = begin(item_mods); it != end(item_mods); it++) 
			{
				if (it->mod_category == "str") 
					ImGui::Text("Increases Strength by %d points", it->mod_value);
				if (it->mod_category == "agl") 
					ImGui::Text("Increases Agility by %d points", it->mod_value);
				if (it->mod_category == "inl") 
					ImGui::Text("Increases Intelligence by %d points", it->mod_value);
				if (it->mod_category == "spd") 
					ImGui::Text("Increases Speed by %d points", it->mod_value);
				if (it->mod_category == "lck") 
					ImGui::Text("Increases Luck by %d points", it->mod_value);
				if (it->mod_category == "maxHP") 
					ImGui::Text("Increases Health Points by %d", it->mod_value);
				if (it->mod_category == "maxMN") 
					ImGui::Text("Increases Mana Points by %d", it->mod_value);
			}
			if (ImGui::Button("Equip Item")) 
			{
				// Since we only want to equip an item to the slot type it will occupy a while loop and bool controls flow
				// Once we find the correct type, we do the following:
				// 		1) Set the appropriate static memory for that slot to contain a dereferenced copy of the Item in the heap
				//		2) Call Player::populate_wquipped_with_static_refs with all the static memory slots - keeps in scope
				//		3) Provide Debug info to show what item type was equipped
				//		4) Update state in static memory for inventoy by calling Player::get_bag() 
				//		5) Set done to true and break out of the while loop

				// There is probably a much better way to do this but MVP is just focused on making it work
				// TODO - find a better way!

				bool done = false;
				bool swapped = false;
				while (!done)
				{
					if (inventory[current_item].get_type() == 0) 
					{
						swapped = armor_slot.get_type() == -1 ? false : true;  // flip swapped to true if this was previously the placeholder empty value
						armor_slot = Player->equip_item(&inventory[current_item]);
						armor_slot = Player->equip_item(&armor_slot);
						Player->populate_equipped_with_static_refs(&armor_slot, &helmet_slot, &glove_slot, &boot_slot, &weapon_slot);
						inventory = Player->get_bag();
						done = true;
						break;
					}

					if (inventory[current_item].get_type() == 1) 
					{
						swapped = helmet_slot.get_type() == -1 ? false : true;  
						helmet_slot = Player->equip_item(&inventory[current_item]);
						helmet_slot = Player->equip_item(&helmet_slot);
						Player->populate_equipped_with_static_refs(&armor_slot, &helmet_slot, &glove_slot, &boot_slot, &weapon_slot);
						inventory = Player->get_bag();
						done = true;
						break;
					}

					if (inventory[current_item].get_type() == 2) 
					{
						swapped = glove_slot.get_type() == -1 ? false : true;  
						glove_slot = Player->equip_item(&inventory[current_item]);
						glove_slot = Player->equip_item(&glove_slot);
						Player->populate_equipped_with_static_refs(&armor_slot, &helmet_slot, &glove_slot, &boot_slot, &weapon_slot);
						inventory = Player->get_bag();
						done = true;
						break;
					}

					if (inventory[current_item].get_type() == 3) 
					{
						swapped = boot_slot.get_type() == -1 ? false : true;  
						boot_slot = Player->equip_item(&inventory[current_item]);
						boot_slot = Player->equip_item(&boot_slot);
						Player->populate_equipped_with_static_refs(&armor_slot, &helmet_slot, &glove_slot, &boot_slot, &weapon_slot);
						inventory = Player->get_bag();
						done = true;
						break;
					}

					if (inventory[current_item].get_type() == 4) 
					{
						swapped = weapon_slot.get_type() == -1 ? false : true;  
						weapon_slot = Player->equip_item(&inventory[current_item]);
						weapon_slot = Player->equip_item(&weapon_slot);
						Player->populate_equipped_with_static_refs(&armor_slot, &helmet_slot, &glove_slot, &boot_slot, &weapon_slot);
						inventory = Player->get_bag();
						done = true;
						break;
					}
				}


				// Iterate through inventory names starting at the former item just equipped and set it to next value
				// This is kind of like a left shift of everything past the current item since we need to match
				// the inventory_names static char array to the location of items in Player::bag
				for (int i = current_item; i < item_count; i++) 
				{
					inventory_names[i] = inventory_names[i + 1];
				}

				// Reset item count to reflect current state unless got swapped
				if (!swapped) 
				{				
					item_count--;
					current_item--;  
					if(current_item == -1) 
						current_item = 0;
				}
			}
		} else {
			// If the user clicks on an item that is outside of the actual displayed items (i.e. the empty string) then we
			// Don't want to try to display anything for that item because it will crash since nothing is at said index
			// We just display "Select an Item" 
			ImGui::Text("Select an Item");
		}
		ImGui::End();

		ImGui::Begin("Equipped Items and Gear");

		// Armor
		if (armor_slot.get_name() == "STUB")
		{ 
			ImGui::Text("Armor:     ");
			ImGui::SameLine();
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text("Armor:     ");
			ImGui::SameLine();
			ImGui::Text(armor_slot.get_name().data());
		}

		// Helmet

		if (helmet_slot.get_name() == "STUB")
		{ 
			ImGui::Text("Helmet:    ");
			ImGui::SameLine();
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text("Helmet:    ");
			ImGui::SameLine();
			ImGui::Text(helmet_slot.get_name().data());
		}

		// Gloves

		if (glove_slot.get_name() == "STUB")
		{ 
			ImGui::Text("Gloves:    ");
			ImGui::SameLine();
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text("Gloves:    ");
			ImGui::SameLine();
			ImGui::Text(glove_slot.get_name().data());
		}

		// Boots

		if (boot_slot.get_name() == "STUB")
		{ 
			ImGui::Text("Boots:     ");
			ImGui::SameLine();
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text("Boots:     ");
			ImGui::SameLine();
			ImGui::Text(boot_slot.get_name().data());
		}

		// Weapon

		if (weapon_slot.get_name() == "STUB")
		{ 
			ImGui::Text("Weapon:    ");
			ImGui::SameLine();
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text("Weapon:    ");
			ImGui::SameLine();
			ImGui::Text(weapon_slot.get_name().data());
		}
		
		ImGui::End();

		// Below are debug windows used to troubleshoot, see the state of a Character or Item, logging, etc.
		// Setting this const to true at the top of this file will open my custom debug window
		// It will also open ImGui demo to use for reference.  

		// Note: this was primarily a tool to help me troubleshoot problems and developed dynamicaly with project
		// Not all buttons work, are hooked up correctly or changed along with the rest of the code.
		// Much instability may insue when using these so turn them on at your own risk :) 

		if (DEBUG_ON) 
		{
			ImGui::Begin("Debug view state");
			ImGui::Text("Click an option below to log to console, note this does not display anything in this window.  Check the console output to see the actual logged contents");

			ImGui::Text("Character");
			if (ImGui::Button("Show Character State")) 
				Player->print_all();
			ImGui::SameLine();

			if (ImGui::Button("Show Equipped Items")) 
				Player->print_equipped();
			ImGui::SameLine();

			if (ImGui::Button("Show Inventory")) 
				Player->print_inventory();
			ImGui::Text("");
			ImGui::Text("Item");
			if (ImGui::Button("Show current selected Item")) 
			{
				std::cout << inventory[current_item].get_name() << std::endl;
				std::cout << "Address: " << &inventory[current_item] << std::endl;
			}
			ImGui::SameLine();
			if (ImGui::Button("Show equipped items")) 
			{
				Item * equipped_items;
				for (int i = 0; i < 4; i++)
					std::cout<<equipped_items[i].get_name()<<std::endl;
				delete equipped_items;
			}
			if (ImGui::Button("show armor")) 
				std::cout<<armor_slot.get_name();
			ImGui::SameLine();
			if (ImGui::Button("show helmet")) 
				std::cout<<helmet_slot.get_name();
			ImGui::SameLine();
			if (ImGui::Button("show gloves")) 
				std::cout<<glove_slot.get_name();
			ImGui::SameLine();
			if (ImGui::Button("show boots")) 
				std::cout<<boot_slot.get_name();
			ImGui::SameLine();
			if (ImGui::Button("show weapon")) 
				std::cout<<weapon_slot.get_name();
			if(ImGui::Button("Maybe print bag???"))
			{
				std::vector<Item> bag = Player->get_bag();
				std::cout << " " << std::endl;
				for (auto it = bag.begin(); it != bag.end(); it++) 
				{
					std::cout<< it->get_name() << std::endl;
				}
			}


			ImGui::End();
			ImGui::ShowDemoWindow();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glfwSwapBuffers(window);
	}


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
