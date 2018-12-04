#pragma once

#include<iostream>
#include<string>
#include<Global_Constants.h>
#include<vector>
#include<deque>

using namespace std;

class Global_Service_Locator;
class Object_Service_Locator;
struct Item_Template;

enum Scaffold_States
{
	BLUEPRINT_STATE_NULL,
	BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT,
	BLUEPRINT_STATE_PARTS_NEEDED_JOB_SENT,
	BLUEPRINT_STATE_NO_PARTS_NEEDED_NO_JOB_SENT,
	BLUEPRINT_STATE_WAITING_TO_START_BUILD,
	BLUEPRINT_BUILD_JOB_SENT
};

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
	int uniq_id = rand() % 10000;
	int part_of_active_job = 0;
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

struct Build_Order
{
	Blueprint* build_requirements;
	int object_type;
	int object_template;
	int build_stage = BLUEPRINT_STATE_NULL;
};

class AI_Item_Component
{
public:
	AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inventory_slots);

	void Update();
	void Update_Entity();
	void Update_Structure();
	void Update_Build_Orders();
	void Check_For_Messages();

	// This is the main function that changes inventory slots - everything must flow through this function
	// so that it can interface with the inventory manifest maintained in the scene graph
	void Alter_Inventory_Slot(int slot_num, int new_quantity, bool item_swap, Item item);

	// These functions utilize the above function to make specific changes
	int Add_Item_To_Inventory(int item_id, int quantity, bool has_stats, Item_Stats item_stats = {});
	int Remove_Item_From_Inventory(int item_id, int quantity);
	
	// These broader functions utilize the above two functions to make changes
	void Copy_Inventory_From_Pointer(Item_Slot pointer[], int num_inventory_slots);
	void Populate_Production_Blueprints(int blueprint_id);
	void Populate_Starter_Inventory(int object_type);
	void Delete_Item_At_Inventory_Array_Num(int array_num);
	bool Remove_Blueprint_Items_From_Inventory(Blueprint* current_blueprint);
	void Clear_All_Inventory();

	// Accessors
	int Return_Num_Inventory_Slots();
	int Return_Num_Occupied_Inventory_Slots();
	Item_Slot* Return_Entire_Inventory_As_Pointer();
	Item_Slot* Return_Inventory_Slot_As_Pointer(int slot_num);
	Item_Slot* Return_First_Inventory_Slot_With_Item_Or_Item_Type(int item_id = 0, string item_type_id = "");
	int Return_Amount_Of_Item_Or_Type_In_Inventory(Item item, string item_type = "");
	int Return_Num_Production_Blueprints();
	Blueprint* Return_Blueprint_At_Slot(int blueprint_slot);

	// Job Related Functions
	void Scan_Inventory_For_Storable_Items(int threshold_quantity);
	void Request_Production_Of_Item_From_Blueprint(Blueprint* blueprint, int quantity);
	void Add_Build_Order_To_Queue(Blueprint* blueprint, int object_type, int object_template, int object_stage = BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
	void Add_Build_Order_To_Queue(Build_Order new_build_order);
	void Create_Production_Object();

	// Queries
	bool Object_Has_Items_For_Blueprint_In_Inventory(Blueprint* blueprint);
	bool Build_Is_Complete();
	void Update_Current_Build_Level(int new_level);
	void Increment_Current_Build_Level(int increment);
	int Return_Current_Build_Level();
	int Return_Build_Level_Target();

	// Specific Functions
	void Prep_Growth_Build_Orders();


private:
	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;
	int object_type = 0;

	// Scaffold Specific Variables
	bool Check_If_Next_Build_Order_Can_Start_Build();
	bool Try_To_Create_Job_To_Fetch_Parts();
	bool Create_Build_Job();

	int item_change_flag = 1;

	int num_inventory_slots = 0;
	Item_Slot inventory_array[MAX_NUM_INVENTORY_SLOTS];
	
	int num_equipment_slots = 0;
	Equipment_Slot equipment_array[MAX_NUM_EQUIPMENT_SLOTS];

	vector<Blueprint*> loaded_production_blueprints;
	deque<Build_Order> active_build_orders;

	int current_build_progress = 0;
	int previous_build_progress = 0;
	int build_target_level = 0;
};