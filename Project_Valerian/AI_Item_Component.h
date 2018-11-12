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
	bool has_stats = false;
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

struct Blueprint
{
	int Blueprint_ID = 0;
	string blueprint_string_name;
	int associated_template_id = 0;
	int associated_object_type = 0;
	int associated_blueprint_type = 0;
	int Num_Items_In_Blueprint = 0;
	Item_Slot blueprint_items[MAX_ITEMS_PER_BLUEPRINT];
};

class AI_Item_Component
{
public:
	AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inventory_slots);

	void Update();

	int Add_Item_To_Inventory(int item_id, int quantity, bool has_stats, Item_Stats item_stats = {});
	int Remove_Item_From_Inventory(int item_id, int quantity);
	void Copy_Inventory_From_Pointer(Item_Slot pointer[], int num_inventory_slots);
	void Populate_Starter_Inventory(int object_type);
	void Delete_Item_At_Inventory_Array_Num(int array_num);
	void Clear_All_Inventory();

	// Accessors
	int Return_Num_Inventory_Slots();
	int Return_Num_Occupied_Inventory_Slots();
	Item_Slot* Return_Entire_Inventory_As_Pointer();
	Item_Slot* Return_Inventory_Slot_As_Pointer(int slot_num);

	// Queries
	int Return_Amount_Of_Item_In_Inventory(Item item);
	bool Object_Has_Items_For_Blueprint_In_Inventory(Blueprint* blueprint);


private:
	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	bool Check_For_Scaffold_Completion();
	int scaffold_build_job_sent = 0;
	int item_change_flag = 1;

	int num_inventory_slots = 0;
	Item_Slot inventory_array[MAX_NUM_INVENTORY_SLOTS];
	
	int num_equipment_slots = 0;
	Equipment_Slot equipment_array[MAX_NUM_EQUIPMENT_SLOTS];

	int num_production_blueprints = 0;
	Blueprint* production_blueprint_array[MAX_NUM_ITEM_BLUEPRINTS];

	int has_scaffold_blueprint = 0;
	Blueprint* Scaffold_Blueprint = NULL;
};