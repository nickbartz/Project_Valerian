#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<algorithm>

AI_Item_Component::AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inv_slots, int equipment_template_id, int blueprint_id, int starter_inventory_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	num_inventory_slots = num_inv_slots;

	if (starter_inventory_id != 0)
	{
		Populate_Starter_Inventory(starter_inventory_id);
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
				inventory_array[i].item_quantity += item_quantity;
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
				inventory_array[i].slot_item.item_template_id = service_locator->get_Game_Library()->Fetch_Item_Template(item_id)->inventory_item_id;
				inventory_array[i].item_quantity = item_quantity;
				if (has_stats) inventory_array[i].slot_item.current_stats = item_stats;
				return 0;
			}
			return 2;
		}
	}
	return 1;
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
			inventory_array[i].item_quantity -= min(num_to_remove, inventory_array[i].item_quantity);
			num_to_remove -= min(inventory_array[i].item_quantity, num_to_remove);
		}
	}

	if (num_to_remove == 0) outcome = 3;
	else if (num_to_remove > 0) outcome = 1;
	else if (num_to_remove < 0) outcome = 2;

	return outcome;
}

void AI_Item_Component::Delete_Item_At_Inventory_Array_Num(int array_num)
{
	inventory_array[array_num].item_quantity = 0;
}

void AI_Item_Component::Clear_All_Inventory()
{
	for (int i = 0; i < num_inventory_slots; i++)
	{
		Delete_Item_At_Inventory_Array_Num(i);
	}
}

void AI_Item_Component::Populate_Starter_Inventory(int starter_id)
{
	Inventory_Template* starter_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(starter_id);

	for (int i = 0; i < MAX_ITEMS_PER_BLUEPRINT; i++)
	{
		if (starter_blueprint->inventory_pack[i][0] != 0)
		{
			int item_id = starter_blueprint->inventory_pack[i][0];
			int quantity = starter_blueprint->inventory_pack[i][1];

			if (Add_Item_To_Inventory(item_id, quantity, false) != 0)
			{
				cout << "trying to add an item that doesn't exist" << endl;
			}
		}
	}
}

void AI_Item_Component::Copy_Inventory_From_Pointer(Item_Slot pointer[], int num_inventory_slots)
{
	for (int i = 0; i < num_inventory_slots; i++)
	{
		if (pointer[i].item_quantity > 0) Add_Item_To_Inventory(pointer[i].slot_item.item_template_id, pointer[i].item_quantity, pointer[i].slot_item.has_stats, pointer[i].slot_item.current_stats);
	}
}