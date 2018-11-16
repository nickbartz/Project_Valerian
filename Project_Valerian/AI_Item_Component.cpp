#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<algorithm>
#include<AI_Stats_Component.h>
#include<Scene_Graph.h>
#include<Message_Array.h>

AI_Item_Component::AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inv_slots)
{
	service_locator = sLocator;
	object_locator = oLocator;

	num_inventory_slots = num_inv_slots;

	switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
	{
	case OBJECT_TYPE_STRUCTURE:
		if (service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->has_starter_inventory >= 1) Populate_Starter_Inventory(OBJECT_TYPE_STRUCTURE);
		break;
	case OBJECT_TYPE_ENTITY:
		if (service_locator->get_Game_Library()->Fetch_Entity_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->entity_has_starter_inventory >= 1)
		{
			Populate_Starter_Inventory(OBJECT_TYPE_ENTITY);
		}
		break;
	case OBJECT_TYPE_SCAFFOLD:
		Scaffold_Blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->scaffold_blueprint_id);
		if (Scaffold_Blueprint == NULL) has_scaffold_blueprint = 0, scaffold_stage = SCAFFOLD_STATE_NO_PARTS_NEEDED_NO_JOB_SENT;
		else scaffold_stage = SCAFFOLD_STATE_PARTS_NEEDED_NO_JOB_SENT, has_scaffold_blueprint = 1;
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

bool AI_Item_Component::Check_For_Scaffold_Completion()
{
	if ((has_scaffold_blueprint == 0 || Object_Has_Items_For_Blueprint_In_Inventory(Scaffold_Blueprint)))
	{
		service_locator->get_Scene_Graph()->Job_Create_Build_Scaffold(object_locator->Return_Object_Pointer());
		return true;
	}
	else return false;
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
		if (Return_Amount_Of_Item_In_Inventory(blueprint->blueprint_items[i].slot_item) < blueprint->blueprint_items[i].item_quantity)
		{
			check = false;
		}
	}

	return check;
}

void AI_Item_Component::Populate_Starter_Inventory(int object_type)
{
	vector<Blueprint*> inventory_starter_blueprints = service_locator->get_Game_Library()->Fetch_All_Blueprints_Of_Type_For_Object(BLUEPRINT_INVENTORY, object_type, object_locator->Return_AI_Stats_Pointer()->Return_Template_ID());

	if (inventory_starter_blueprints.size() > 0)
	{
		Blueprint* starter_blueprint = inventory_starter_blueprints.back();

		for (int i = 0; i < starter_blueprint->Num_Items_In_Blueprint; i++)
		{
			Add_Item_To_Inventory(starter_blueprint->blueprint_items[i].slot_item.item_template_id, starter_blueprint->blueprint_items[i].item_quantity, false, {});
		}
	}
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

int AI_Item_Component::Return_Amount_Of_Item_In_Inventory(Item item)
{
	int amount_of_item_in_inventory = 0;

	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (inventory_array[i].slot_item.item_template_id == item.item_template_id)
		{
			amount_of_item_in_inventory += inventory_array[i].item_quantity;
		}
	}

	return amount_of_item_in_inventory;
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
		// EDIT THIS EVENTUALLY FOR MORE THAN JUST ORE
		if (service_locator->get_Game_Library()->Fetch_Item_Template(inventory_array[i].slot_item.item_template_id)->inventory_item_type == "INVENTORY_ITEM_TYPE_ORE" && inventory_array[i].item_quantity > 0 && inventory_array[i].part_of_active_job == 0)
		{
			if (new_blueprint.Num_Items_In_Blueprint < MAX_ITEMS_PER_BLUEPRINT)
			{
				Item found_ore;
				found_ore.item_template_id = inventory_array[i].slot_item.item_template_id;
				new_blueprint.blueprint_items[new_blueprint.Num_Items_In_Blueprint].slot_item = found_ore;
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
		service_locator->get_Scene_Graph()->Job_Create_Transport_Blueprint_Items_From_Object_To_Requestee(object_locator->Return_Object_Pointer(), nearest_storage, new_blueprint, 0, object_locator->Return_Object_Pointer());
		for (int i = 0; i < slot_pointers.size(); i++)
		{
			slot_pointers[i]->part_of_active_job = 1;
		}
	}
}

bool AI_Item_Component::Try_To_Create_Job_To_Fetch_Parts()
{
	if (service_locator->get_Scene_Graph()->Job_Create_Find_Blueprint_Items_And_Transport_To_Requestee(service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(OBJECT_TYPE_SCAFFOLD, object_locator->Return_Object_Pointer()->Get_Array_Index()), Scaffold_Blueprint, true) == true) return true;
	else return false;
}

void AI_Item_Component::Update()
{
	if (item_change_flag > 0)
	{
		switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
		{
		case OBJECT_TYPE_ENTITY:
			Update_Entity();
			break;
		case OBJECT_TYPE_SCAFFOLD:
			Update_Scaffold();
			break;
		}
	}
}

void AI_Item_Component::Update_Entity()
{
	if (object_locator->Return_AI_Job_Pointer()->Return_Current_Num_Goals() == 0)
	{
		Scan_Inventory_For_Storable_Items(0);
		item_change_flag = 0;
	}
}

void AI_Item_Component::Update_Scaffold()
{
	if (scaffold_stage == SCAFFOLD_STATE_NO_PARTS_NEEDED_NO_JOB_SENT)
	{
		if (Check_For_Scaffold_Completion() == true) scaffold_stage = SCAFFOLD_STATE_NO_PARTS_NEEDED_JOB_SENT;
	}
	else if (scaffold_stage == SCAFFOLD_STATE_PARTS_NEEDED_NO_JOB_SENT)
	{
		if (Try_To_Create_Job_To_Fetch_Parts() == true) scaffold_stage = SCAFFOLD_STATE_PARTS_NEEDED_JOB_SENT;
		else scaffold_stage = SCAFFOLD_STATE_PARTS_NEEDED_NO_JOB_SENT;
	}
	else if (scaffold_stage == SCAFFOLD_STATE_PARTS_NEEDED_JOB_SENT)
	{
		if (Check_For_Scaffold_Completion() == true) scaffold_stage = SCAFFOLD_STATE_NO_PARTS_NEEDED_JOB_SENT;
	}

	item_change_flag = 0;
}

