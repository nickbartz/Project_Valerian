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

Item_Slot AI_Item_Component::Return_Item_In_Slot(int item_id)
{
	return inventory_array[item_id];
}

int AI_Item_Component::Add_Item_To_Inventory(int item_id, int item_quantity, bool has_stats, Item_Stats item_stats)
{
	int result = 1;
	// Result is a message that corresponds to the outcome of the attempt to add an inventory item
	// 0 means the item was added; 
	// 1 means the item was not added because there wasn't a free slot
	// 2 means the item was not added because it didn't exist

	for (int i = 0; i < min(num_inventory_slots, MAX_NUM_INVENTORY_SLOTS); i++)
	{
		if (inventory_array[i].item_quantity <= 0 || inventory_array[i].slot_item.item_template_id == 0)
		{	
			if (service_locator->get_Game_Library()->Fetch_Item_Template(item_id).inventory_item_id != 0)
			{
				inventory_array[i].slot_item.item_template_id = service_locator->get_Game_Library()->Fetch_Item_Template(item_id).inventory_item_id;
				inventory_array[i].item_quantity = item_quantity;
				if (has_stats) inventory_array[i].slot_item.current_stats = item_stats;
				return 0;
			}
			return 2;
		}
	}
	return 1;
}

void AI_Item_Component::Populate_Starter_Inventory(int starter_id)
{
	Inventory_Template starter_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(starter_id);

	for (int i = 0; i < MAX_NUM_ITEMS_IN_BLUEPRINT; i++)
	{
		if (starter_blueprint.inventory_pack[i][0] != 0)
		{
			int item_id = starter_blueprint.inventory_pack[i][0];
			int quantity = starter_blueprint.inventory_pack[i][1];

			if (Add_Item_To_Inventory(item_id, quantity, false) != 0)
			{
				cout << "trying to add an item that doesn't exist" << endl;
			}
		}
	}
}