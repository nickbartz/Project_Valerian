#pragma once

#include<iostream>
#include<string>
#include<Global_Constants.h>
using namespace std;

class Global_Service_Locator;
class Object_Service_Locator;
struct Item_Template;


struct Item_Stats
{

};

struct Item
{
	int item_template_id = 0; // Item ID links to the Game Library so we don't have to waste memory repeating templated information when it can be looked up
	Item_Stats current_stats; // Store a local copy of current stats so they can change;
};

struct Item_Slot
{
	Item slot_item;
	int item_quantity = 0;
};

struct Equipment_Slot
{
	string Equipment_Slot_Name;
	int Equipment_Slot_ID = 0;
	int Stackable = 0;
	Item_Slot item;
};

struct Blueprint_Slot
{
	int Blueprint_ID = 0;
	int Num_Items_In_Blueprint = 0;
	Item_Slot blueprint_items[MAX_ITEMS_PER_BLUEPRINT];
};

class AI_Item_Component
{
public:
	AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inventory_slots, int equipment_template_id, int blueprint_id, int starter_inventory_id);

	int Add_Item_To_Inventory(int item_id, int quantity, bool has_stats, Item_Stats item_stats = {});
	void Populate_Starter_Inventory(int starter_id);

	// Accessors
	int Return_Num_Inventory_Slots();
	int Return_Num_Occupied_Inventory_Slots();
	Item_Slot* Return_Inventory_Slot_As_Pointer(int slot_num);

private:
	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	void Copy_Inventory_From_Pointer(Item_Slot pointer[], int num_inventory_slots);

	int num_inventory_slots = 0;
	Item_Slot inventory_array[MAX_NUM_INVENTORY_SLOTS];
	
	int num_equipment_slots = 0;
	Equipment_Slot equipment_array[MAX_NUM_EQUIPMENT_SLOTS];

	int num_blueprints = 0;
	Blueprint_Slot blueprint_array[MAX_NUM_ITEM_BLUEPRINTS];
};