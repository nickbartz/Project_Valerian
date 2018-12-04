#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<algorithm>
#include<AI_Stats_Component.h>
#include<Scene_Graph.h>
#include<Message_Array.h>
#include<AI_Movement_Component.h>

AI_Item_Component::AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inv_slots)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = object_locator->Return_AI_Stats_Pointer()->Return_Object_Type();

	num_inventory_slots = num_inv_slots;

	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		if (service_locator->get_Game_Library()->Fetch_Structure_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->has_starter_inventory >= 1) Populate_Starter_Inventory(OBJECT_TYPE_STRUCTURE);
		if (service_locator->get_Game_Library()->Fetch_Structure_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->blueprint_pack_id != 0)
		{
			Populate_Production_Blueprints(service_locator->get_Game_Library()->Fetch_Structure_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->blueprint_pack_id);
		}
		
		// There's got to be a simpler way to do this, but I don't see myself making the user go in and manually select all five growth stages for each frenzel tile
		if (object_locator->Return_AI_Stats_Pointer()->Get_Structure_Type() == service_locator->get_Game_Library()->Get_Structure_Type_Code_From_Structure_Type_String("STRUCTURE_TYPE_LIVING_FRENZEL"))
		{
			Prep_Growth_Build_Orders();
		}

		break;
	case OBJECT_TYPE_ENTITY:
		if (service_locator->get_Game_Library()->Fetch_Entity_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->entity_starter_inventory_blueprint >= 1)
		{
			Populate_Starter_Inventory(service_locator->get_Game_Library()->Fetch_Entity_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->entity_starter_inventory_blueprint);
		}
		break;
	case OBJECT_TYPE_SCAFFOLD:
		Blueprint* scaffold_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(service_locator->get_Game_Library()->Fetch_Structure_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->scaffold_blueprint_id);
		if (scaffold_blueprint != NULL)
		{
			Add_Build_Order_To_Queue(scaffold_blueprint, OBJECT_TYPE_STRUCTURE, object_locator->Return_AI_Stats_Pointer()->Return_Template_ID(), BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
		}
		else
		{
			Add_Build_Order_To_Queue(NULL, OBJECT_TYPE_STRUCTURE, object_locator->Return_AI_Stats_Pointer()->Return_Template_ID(), BLUEPRINT_STATE_WAITING_TO_START_BUILD);
		}
		break;
	}

}

void AI_Item_Component::Alter_Inventory_Slot(int slot_num, int new_quantity, bool item_swap, Item item)
{
	// If a new item is being added to an empty slot, add a new item slot to the manifest
	if (inventory_array[slot_num].item_quantity <= 0 && new_quantity > 0)
	{
		service_locator->get_Scene_Graph()->Add_Inventory_Slot_To_Manifest(&inventory_array[slot_num], object_locator->Return_Object_Pointer());
	}
	// If an item slot is being reduced to zero, remove it from the manifest
	else if (inventory_array[slot_num].item_quantity > 0 && new_quantity <= 0)
	{
		service_locator->get_Scene_Graph()->Remove_Inventory_Slot_From_Manifest(&inventory_array[slot_num]);
	}

	// Actually make the changes to the inventory slot
	inventory_array[slot_num].item_quantity = new_quantity;
	if (item_swap) inventory_array[slot_num].slot_item = item;

	// If the inventory slot was involved in an active job, flag it as no longer part of that job. This could lead to undesirable consequences but no better option right now
	if (inventory_array[slot_num].part_of_active_job == 1)
	{
		inventory_array[slot_num].part_of_active_job = 0;
	}

	item_change_flag = 1;

}

int AI_Item_Component::Add_Item_To_Inventory(int item_id, int item_quantity, bool has_stats, Item_Stats item_stats)
{
	int result = 1;
	// Result is a message that corresponds to the outcome of the attempt to add an inventory item
	// 0 means the item was added to a new slot; 
	// 1 means the item was not added because there wasn't a free slot
	// 2 means the item was not added because it didn't exist
	// 3 means the item was added to an existing slot

	// Check To See If the Item Is Already In the Inventory
	if (has_stats == false)
	{
		for (int i = 0; i < min(num_inventory_slots, MAX_NUM_INVENTORY_SLOTS); i++)
		{
			if (inventory_array[i].slot_item.item_template_id == item_id)
			{
				Alter_Inventory_Slot(i, inventory_array[i].item_quantity + item_quantity, false, {});
				return 3;
			}
		}
	}

	// Add Items That Aren't Already In the Inventory
	for (int i = 0; i < min(num_inventory_slots, MAX_NUM_INVENTORY_SLOTS); i++)
	{
		if (inventory_array[i].item_quantity <= 0 || inventory_array[i].slot_item.item_template_id == 0)
		{
			if (service_locator->get_Game_Library()->Fetch_Item_Template(item_id)->inventory_item_id != 0)
			{
				Item new_item;
				new_item.item_template_id = service_locator->get_Game_Library()->Fetch_Item_Template(item_id)->inventory_item_id;
				if (has_stats) new_item.current_stats = item_stats;

				Alter_Inventory_Slot(i, item_quantity, true, new_item);
				return 0;
			}
			return 2;
		}
	}
	return 1;
}

void AI_Item_Component::Add_Build_Order_To_Queue(Blueprint* blueprint, int object_type, int object_template, int object_stage)
{
	active_build_orders.push_back(Build_Order{ blueprint, object_type, object_template, object_stage });
}

void AI_Item_Component::Add_Build_Order_To_Queue(Build_Order new_build_order)
{
	active_build_orders.push_back(new_build_order);
}

bool AI_Item_Component::Build_Is_Complete()
{
	if (build_target_level > 0 && current_build_progress > 0)
	{
		if (current_build_progress >= build_target_level)
		{
			return true;
		}
	}

	else return false;
}

void AI_Item_Component::Copy_Inventory_From_Pointer(Item_Slot pointer[], int num_inventory_slots)
{
	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (pointer[i].item_quantity > 0) Add_Item_To_Inventory(pointer[i].slot_item.item_template_id, pointer[i].item_quantity, pointer[i].slot_item.has_stats, pointer[i].slot_item.current_stats);
	}
}

void AI_Item_Component::Check_For_Messages()
{
	Custom_Message* new_message;

	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
		if (new_message->Read_Message(0) == MESSAGE_TYPE_SG_INVENTORY_MANIFEST_UPDATE)
		{
			item_change_flag = 1;
		}
	}
}

void AI_Item_Component::Clear_All_Inventory()
{
	for (int i = 0; i < num_inventory_slots; i++)
	{
		Delete_Item_At_Inventory_Array_Num(i);
	}
}

bool AI_Item_Component::Check_If_Next_Build_Order_Can_Start_Build()
{
	if ((active_build_orders.front().build_requirements == NULL || Object_Has_Items_For_Blueprint_In_Inventory(active_build_orders.front().build_requirements)))
	{
		return true;
	}
	else return false;
}

bool AI_Item_Component::Create_Build_Job()
{
	switch (active_build_orders.front().object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		build_target_level = service_locator->get_Game_Library()->Fetch_Structure_Template(active_build_orders.front().object_template)->max_built_level;
		break;
	case OBJECT_TYPE_ITEM:
		build_target_level = service_locator->get_Game_Library()->Fetch_Item_Template(active_build_orders.front().object_template)->item_build_time;
		break;
	}

	switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
	{
	case OBJECT_TYPE_SCAFFOLD:
		service_locator->get_Scene_Graph()->Job_Create_Entity_Go_Change_Object_Stat(object_locator->Return_Object_Pointer(), 1, NULL, OBJECT_TYPE_SCAFFOLD, STAT_STRUCTURE_BUILT_LEVEL, HIGHER_THAN_OR_EQUAL_TO, 1, build_target_level);
		break;
	case OBJECT_TYPE_STRUCTURE:
		service_locator->get_Scene_Graph()->Job_Create_Entity_Go_Change_Object_Stat(object_locator->Return_Object_Pointer(), 1, NULL, OBJECT_TYPE_STRUCTURE, STAT_STRUCTURE_BUILT_LEVEL, HIGHER_THAN_OR_EQUAL_TO, 1, build_target_level);
		break;
	}

	current_build_progress = 0;
	active_build_orders.front().build_stage = BLUEPRINT_BUILD_JOB_SENT;
	return true;
}

void AI_Item_Component::Create_Production_Object()
{
	int object_template = active_build_orders.front().object_template;

	switch (active_build_orders.front().object_type)
	{
	case OBJECT_TYPE_ITEM:
		{Coordinate object_coordinate = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
		Coordinate nearest_open_coordinate = service_locator->get_Scene_Graph()->Return_Nearest_Accessible_Coordinate(object_coordinate, object_coordinate, object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION));

		Item_Slot production_item;
		production_item.item_quantity = 1;
		production_item.slot_item.item_template_id = active_build_orders.front().object_template;

		service_locator->get_Scene_Graph()->Create_Container(nearest_open_coordinate, &production_item, 1, 1); }
		break;
	case OBJECT_TYPE_STRUCTURE:
		service_locator->get_Scene_Graph()->Delete_Object(object_locator->Return_AI_Stats_Pointer()->Return_Object_Type(), object_locator->Return_Object_Pointer()->Get_Array_Index());
		Object* new_structure = service_locator->get_Scene_Graph()->Create_New_Structure(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object_template, object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION), true);
		
		if (new_structure != NULL)
		{
			Clear_All_Inventory();

			for (int i = 1; i < active_build_orders.size(); i++)
			{
				new_structure->Return_AI_Item_Component()->Add_Build_Order_To_Queue(active_build_orders[i]);
			}
		}

		break;
	}

}

void AI_Item_Component::Delete_Item_At_Inventory_Array_Num(int array_num)
{
	Alter_Inventory_Slot(array_num, 0, false, {});
}

bool AI_Item_Component::Object_Has_Items_For_Blueprint_In_Inventory(Blueprint* blueprint)
{
	bool check = true;
	for (int i = 0; i < blueprint->Num_Items_In_Blueprint; i++)
	{
		if (Return_Amount_Of_Item_Or_Type_In_Inventory(blueprint->blueprint_items[i].slot_item) < blueprint->blueprint_items[i].item_quantity)
		{
			check = false;
		}
	}

	return check;
}

void AI_Item_Component::Populate_Production_Blueprints(int blueprint_id)
{
	Blueprint_Pack* blueprint_pack = service_locator->get_Game_Library()->Fetch_Blueprint_Pack(blueprint_id);

	if (blueprint_pack != NULL)
	{
		for (int i = 0; i < blueprint_pack->blueprint_id_array.size(); i++)
		{
			loaded_production_blueprints.push_back(service_locator->get_Game_Library()->Fetch_Blueprint(blueprint_pack->blueprint_id_array[i]));
		}
	}
	else
	{
		cout << "trying to populate with NULL blueprint pack" << endl;
	}
}

void AI_Item_Component::Populate_Starter_Inventory(int object_inventory_blueprint)
{
	Blueprint* inventory_starter_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(object_inventory_blueprint);

	for (int i = 0; i < inventory_starter_blueprint->Num_Items_In_Blueprint; i++)
	{
		int item_id = inventory_starter_blueprint->blueprint_items[i].slot_item.item_template_id;
		int item_quantity = inventory_starter_blueprint->blueprint_items[i].item_quantity;

		Add_Item_To_Inventory(item_id, item_quantity, false, {});
	}
}

void AI_Item_Component::Prep_Growth_Build_Orders()
{
	if (loaded_production_blueprints.size() > 1)
	{
		Blueprint* level_1_blueprint = loaded_production_blueprints[0];
		Blueprint* level_2_blueprint = loaded_production_blueprints[1];
		Blueprint* level_3_blueprint = loaded_production_blueprints[2];
		Blueprint* level_4_blueprint = loaded_production_blueprints[3];
		Blueprint* level_5_blueprint = loaded_production_blueprints[4];

		if (level_1_blueprint != NULL && level_2_blueprint != NULL && level_3_blueprint != NULL && level_4_blueprint != NULL && level_5_blueprint != NULL)
		{
			Add_Build_Order_To_Queue(level_1_blueprint, OBJECT_TYPE_STRUCTURE, level_1_blueprint->associated_template_id, BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
			Add_Build_Order_To_Queue(level_2_blueprint, OBJECT_TYPE_STRUCTURE, level_2_blueprint->associated_template_id, BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
			Add_Build_Order_To_Queue(level_3_blueprint, OBJECT_TYPE_STRUCTURE, level_3_blueprint->associated_template_id, BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
			Add_Build_Order_To_Queue(level_4_blueprint, OBJECT_TYPE_STRUCTURE, level_4_blueprint->associated_template_id, BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
			Add_Build_Order_To_Queue(level_5_blueprint, OBJECT_TYPE_ITEM,	   level_5_blueprint->associated_template_id, BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT);
		}
	}
}

void AI_Item_Component::Request_Production_Of_Item_From_Blueprint(Blueprint* blueprint, int quantity)
{
	int stage = BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT;
	if (blueprint->Num_Items_In_Blueprint == 0) stage == BLUEPRINT_STATE_NO_PARTS_NEEDED_NO_JOB_SENT;

	for (int i = 0; i < quantity; i++)
	{
		Add_Build_Order_To_Queue(blueprint, OBJECT_TYPE_ITEM, blueprint->associated_template_id, stage);
	}
}

bool AI_Item_Component::Remove_Blueprint_Items_From_Inventory(Blueprint* current_blueprint)
{
	for (int i = 0; i < current_blueprint->Num_Items_In_Blueprint; i++)
	{
		if (Remove_Item_From_Inventory(current_blueprint->blueprint_items[i].slot_item.item_template_id, current_blueprint->blueprint_items[i].item_quantity) != 3)
		{
			return false;
			cout << "something went wrong while removing a blueprint items from inventory" << endl;
		}
	}

	return true;
}

int AI_Item_Component::Return_Num_Inventory_Slots()
{
	return num_inventory_slots;
}

int AI_Item_Component::Return_Num_Occupied_Inventory_Slots()
{
	int count = 0;
	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (inventory_array[i].item_quantity > 0) count++;
	}

	return count;
}

Item_Slot* AI_Item_Component::Return_Entire_Inventory_As_Pointer()
{
	return inventory_array;
}

Item_Slot* AI_Item_Component::Return_Inventory_Slot_As_Pointer(int item_id)
{
	return &inventory_array[item_id];
}

int AI_Item_Component::Return_Num_Production_Blueprints()
{
	return loaded_production_blueprints.size();
}

Blueprint* AI_Item_Component::Return_Blueprint_At_Slot(int blueprint_slot)
{
	if (blueprint_slot < loaded_production_blueprints.size())
	{
		return loaded_production_blueprints[blueprint_slot];
	}
	else
	{
		cout << "blueprint slot called out of range" << endl;
		return NULL;
	}

}

int AI_Item_Component::Return_Amount_Of_Item_Or_Type_In_Inventory(Item item, string item_type)
{
	if (item_type.size() > 0 && service_locator->get_Game_Library()->Get_Item_Type_Code_From_Item_Type_String(item_type) == 0)
	{
		return 0;
	}

	int amount_of_item_in_inventory = 0;

	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (item_type.size() == 0)
		{
			if (inventory_array[i].slot_item.item_template_id == item.item_template_id)
			{
				amount_of_item_in_inventory += inventory_array[i].item_quantity;
			}
		}
		else
		{
			if (service_locator->get_Game_Library()->Fetch_Item_Template(inventory_array[i].slot_item.item_template_id)->inventory_item_type_string == item_type)
			{
				amount_of_item_in_inventory += inventory_array[i].item_quantity;
			}
		}

	}

	return amount_of_item_in_inventory;
}

Item_Slot* AI_Item_Component::Return_First_Inventory_Slot_With_Item_Or_Item_Type(int item_id, string item_type_string)
{
	if (item_type_string.size() > 0 && service_locator->get_Game_Library()->Get_Item_Type_Code_From_Item_Type_String(item_type_string) == 0)
	{
		return NULL;
	}

	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (item_type_string.size() == 0)
		{
			if (inventory_array[i].slot_item.item_template_id == item_id && inventory_array[i].item_quantity > 0)
			{
				return &inventory_array[i];
			}
		}
		else
		{
			if (service_locator->get_Game_Library()->Fetch_Item_Template(inventory_array[i].slot_item.item_template_id)->inventory_item_type_string == item_type_string && inventory_array[i].item_quantity > 0)
			{
				return &inventory_array[i];
			}
		}

	}

	return NULL;
}

int  AI_Item_Component::Remove_Item_From_Inventory(int item_id, int quantity)
{
	int outcome = 0;

	// 0: Something happened that wasn't supposed to
	// 1: couldn't delete full amount of item
	// 2: Somehow deleted too much of item
	// 3: Deleted the right amount of the item

	int num_to_remove = quantity;
	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (inventory_array[i].slot_item.item_template_id == item_id && inventory_array[i].item_quantity > 0)
		{
			Alter_Inventory_Slot(i, inventory_array[i].item_quantity - min(num_to_remove, inventory_array[i].item_quantity), false, {});
			num_to_remove -= min(inventory_array[i].item_quantity, num_to_remove);
		}
	}

	if (num_to_remove == 0) outcome = 3;
	else if (num_to_remove > 0) outcome = 1;
	else if (num_to_remove < 0) outcome = 2;

	return outcome;
}

void AI_Item_Component::Scan_Inventory_For_Storable_Items(int threshold_quantity)
{
	Blueprint new_blueprint;

	vector<Item_Slot*> slot_pointers;
	int item_quantity = 0;

	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (inventory_array[i].item_quantity > 0 && inventory_array[i].part_of_active_job == 0)
		{
			if (new_blueprint.Num_Items_In_Blueprint < MAX_ITEMS_PER_BLUEPRINT)
			{
				Item storable_item;
				storable_item.item_template_id = inventory_array[i].slot_item.item_template_id;
				new_blueprint.blueprint_items[new_blueprint.Num_Items_In_Blueprint].slot_item = storable_item;
				new_blueprint.blueprint_items[new_blueprint.Num_Items_In_Blueprint].item_quantity = inventory_array[i].item_quantity;
				item_quantity += inventory_array[i].item_quantity;
				new_blueprint.Num_Items_In_Blueprint++;
				slot_pointers.push_back(&inventory_array[i]);
			}
			else break;
		}
	}

	Object* nearest_storage = service_locator->get_Scene_Graph()->Return_Nearest_Structure_By_Type(object_locator->Return_Object_Pointer(), "STRUCTURE_TYPE_STORAGE");

	if (nearest_storage != NULL && new_blueprint.Num_Items_In_Blueprint > 0 && item_quantity >= threshold_quantity)
	{
		service_locator->get_Scene_Graph()->Job_Create_Transport_Blueprint_Items_From_Object_To_Requestee(object_locator->Return_Object_Pointer(), nearest_storage, new_blueprint,0, object_locator->Return_Object_Pointer());
		for (int i = 0; i < slot_pointers.size(); i++)
		{
			slot_pointers[i]->part_of_active_job = 1;
		}
	}
}

bool AI_Item_Component::Try_To_Create_Job_To_Fetch_Parts()
{
	Scene_Graph* scene_graph = service_locator->get_Scene_Graph();
	int object_type = object_locator->Return_AI_Stats_Pointer()->Return_Object_Type();
	if (scene_graph->Job_Create_Find_Blueprint_Items_And_Transport_To_Requestee(scene_graph->Return_Object_By_Type_And_Array_Num(object_type, object_locator->Return_Object_Pointer()->Get_Array_Index()), active_build_orders.front().build_requirements, true) == true)
	{
		active_build_orders.front().build_stage = BLUEPRINT_STATE_WAITING_TO_START_BUILD;
		return true;
	}
	else
	{
		active_build_orders.front().build_stage = BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT;
		return false;
	}
}

void AI_Item_Component::Update()
{
	if (item_change_flag > 0 || previous_build_progress != current_build_progress)
	{
		switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
		{
		case OBJECT_TYPE_ENTITY:
			Update_Entity();
			break;
		case OBJECT_TYPE_STRUCTURE:
			Update_Structure();
			break;
		}

		if (active_build_orders.size() > 0) Update_Build_Orders();
	}

	previous_build_progress = current_build_progress;

}

void AI_Item_Component::Update_Entity()
{
	if (object_locator->Return_AI_Job_Pointer()->Return_Current_Num_Goals() == 0)
	{
		// Removing this for now until we can find a sustainable way to scan inventory for only storeable items
		//Scan_Inventory_For_Storable_Items(0);
	}
}

void AI_Item_Component::Update_Structure()
{
	//if (object_locator->Return_AI_Stats_Pointer()->Get_Structure_Type() == service_locator->get_Game_Library()->Get_Structure_Type_Code_From_Structure_Type_String("STRUCTURE_TYPE_LIVING_FRENZEL"))
	//{

	//}
}

void AI_Item_Component::Update_Build_Orders()
{
	if (active_build_orders.size() > 0)
	{
		switch (active_build_orders.front().build_stage)
		{
		case BLUEPRINT_STATE_PARTS_NEEDED_NO_JOB_SENT:
			Try_To_Create_Job_To_Fetch_Parts();
			break;
		case BLUEPRINT_STATE_WAITING_TO_START_BUILD:
			if (Check_If_Next_Build_Order_Can_Start_Build() == true) Create_Build_Job();
			break;
		case BLUEPRINT_BUILD_JOB_SENT:
			if (Build_Is_Complete() == true)
			{
				if (active_build_orders.front().build_requirements != NULL) Remove_Blueprint_Items_From_Inventory(active_build_orders.front().build_requirements);
				Create_Production_Object();
				active_build_orders.pop_front();
			}
			break;
		}
	}
}

int AI_Item_Component::Return_Current_Build_Level()
{
	return current_build_progress;
}

int AI_Item_Component::Return_Build_Level_Target()
{
	return build_target_level;
}

void AI_Item_Component::Increment_Current_Build_Level(int increment)
{
	current_build_progress += increment;
}

void AI_Item_Component::Update_Current_Build_Level(int new_level)
{
	current_build_progress = new_level;
}