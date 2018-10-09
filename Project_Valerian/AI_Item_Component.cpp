#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Game_Library.h>

AI_Item_Component::AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inv_slots, int equipment_template_id, int blueprint_id, int starter_inventory_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	num_inventory_slots = num_inv_slots;

	Populate_Starter_Inventory(starter_inventory_id);
}

void AI_Item_Component::Populate_Starter_Inventory(int starter_id)
{
	Inventory_Template starter_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(starter_id);
	
	for (int i = 0; i < 5; i++)
	{
		inventory_array[i].slot_item.item_template_id = starter_blueprint.inventory_pack[i][0];
		inventory_array[i].item_quantity = starter_blueprint.inventory_pack[i][1];
	}
}