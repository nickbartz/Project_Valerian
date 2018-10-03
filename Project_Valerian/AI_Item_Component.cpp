#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Game_Library.h>

AI_Item_Component::AI_Item_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int num_inv_slots, int equipment_template_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	num_inventory_slots = num_inv_slots;

}