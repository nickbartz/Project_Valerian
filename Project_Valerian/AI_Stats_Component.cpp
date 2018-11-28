#include<AI_Stats_Component.h>
#include<iostream>
#include<Render_Component.h>
#include<Coordinate.h>
#include<Message_Array.h>
#include<Message.h>
#include<AI_Movement_Component.h>
#include<AI_Job_Component.h>
#include<Scene_Graph.h>
#include<AI_Item_Component.h>


using namespace std;

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int oType, int object_template_num)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = oType;

	switch (oType)
	{
	case OBJECT_TYPE_STRUCTURE:
		object_template_id = object_template_num;
		object_stats.structure_stats.impassable = service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_num)->is_inaccessible;
		break;
	case OBJECT_TYPE_ENTITY:
		object_template_id = object_template_num;
		object_stats.entity_stats.entity_sex = rand() % 3;
		object_stats.entity_first_name = Generate_Entity_Name();
		object_stats.entity_last_name = Generate_Entity_Name();
		object_stats.entity_stats.Create_Dynamic_Stat(STAT_ENTITY_HUNGER, 0, 50, 1, 50, 100, 100);
		object_stats.entity_stats.Create_Dynamic_Stat(STAT_ENTITY_TIREDNESS, 0, 1000, 1, 50, 100,100);
		object_stats.entity_stats.Create_Dynamic_Stat(STAT_ENTITY_OXYGEN, 0, 500, 1, 50, 100,100);
		object_stats.entity_stats.Create_Dynamic_Stat(STAT_ENTITY_FEAR, 0, 1000, 1, 50, 100,100);
		object_stats.entity_stats.Create_Dynamic_Stat(STAT_ENTITY_ENNUI, 0, 1000, 1, 50, 100,100);
		break;
	case OBJECT_TYPE_PROJECTILE:
		object_template_id = object_template_num;
		object_stats.projectile_stats.remaining_lifespan = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_num)->projectile_range;
		break;
	case OBJECT_TYPE_CONTAINER:
		break;
	case OBJECT_TYPE_SCAFFOLD:
		object_template_id = object_template_num;
		object_stats.structure_stats.impassable = service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_num)->is_inaccessible;
		break;
	}

	Assign_Uniq_IDs(object_array_index);
}

void AI_Stats_Component::Assign_Uniq_IDs(int object_array_index)
{
	object_array_locator = object_array_index;
	uniq_id = rand() % 4000;
}

string AI_Stats_Component::Generate_Entity_Name()
{
	string name;
	Game_Library* gl = service_locator->get_Game_Library();

	switch (object_stats.entity_stats.entity_sex)
	{
	case ENTITY_MALE:
		name = gl->Fetch_Random_Name_Syllable(ENTITY_MALE, 0);
		if (rand()% 1 == 1) name += gl->Fetch_Random_Name_Syllable(ENTITY_MALE, 1);
		name += gl->Fetch_Random_Name_Syllable(ENTITY_MALE, 2);
		break;
	case ENTITY_FEMALE:
		name = gl->Fetch_Random_Name_Syllable(ENTITY_FEMALE, 0);
		if (rand() % 1 == 1) name += gl->Fetch_Random_Name_Syllable(ENTITY_FEMALE, 1);
		name += gl->Fetch_Random_Name_Syllable(ENTITY_FEMALE, 2);
		break;
	case ENTITY_NEUTRAL:
		name = gl->Fetch_Random_Name_Syllable(ENTITY_NEUTRAL, 0);
		if (rand() % 1 == 1) name += gl->Fetch_Random_Name_Syllable(ENTITY_NEUTRAL, 1);
		name += gl->Fetch_Random_Name_Syllable(ENTITY_NEUTRAL, 2);
		break;
	}

	return name;
}

string AI_Stats_Component::Get_Structure_Common_Name()
{
	return service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_id)->structure_name;
}

int AI_Stats_Component::Get_Structure_Name()
{
	return service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_id)->structure_id;
}

int AI_Stats_Component::Get_Structure_Type()
{
	return service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_id)->structure_type;
}

string AI_Stats_Component::Get_Entity_Name()
{
	return object_stats.entity_last_name + ", " + object_stats.entity_first_name;
}

void AI_Stats_Component::Update()
{
	// Increment the update cycle 
	update_cycle += 1;
	if (update_cycle > 1000) update_cycle = 0;

	if (!Check_For_Death())
	{
		// RUN OBJECT AI
		switch (object_type)
		{
		case OBJECT_TYPE_ENTITY:
			Update_Entity();
			break;
		case OBJECT_TYPE_STRUCTURE:
			break;
		case OBJECT_TYPE_PROJECTILE:
			Adjust_Stat(STAT_PROJECTILE_RANGE, -1);
			break;
		}
	}
}

void AI_Stats_Component::Update_Entity()
{
	Increment_Dynamic_Stats();
}

void AI_Stats_Component::Update_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_OBJECT_FACTION:
		object_stats.object_faction = new_value;
		break;
	case STAT_STRUCTURE_BUILT_LEVEL:
		object_stats.structure_stats.built_level = new_value;
		break;
	case STAT_OBJECT_HEALTH:
		object_stats.object_health = new_value;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		object_stats.structure_stats.oxygen_level = new_value;
		break;
	case STAT_STRUCTURE_POWERED:
		object_stats.structure_stats.powered = new_value;
		break;
	case STAT_STRUCTURE_IMPASSABLE:
		object_stats.structure_stats.impassable = new_value;
		break;
	case STAT_ENTITY_HUNGER:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER)->stat_number = new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_TIREDNESS)->stat_number = new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_OXYGEN)->stat_number = new_value;
		break;
	case STAT_ENTITY_FEAR:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_FEAR)->stat_number = new_value;
		break;
	case STAT_ENTITY_SPEED:
		object_stats.entity_stats.speed = new_value;
		break;
	case STAT_ENTITY_ENNUI:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_ENNUI)->stat_number = new_value;
		break;
	case STAT_PROJECTILE_RANGE:
		object_stats.projectile_stats.remaining_lifespan = new_value;
		break;
	}
}

void AI_Stats_Component::Handle_Stat_Message(Custom_Message* custom_message)
{
	switch (custom_message->Read_Message(1))
	{
	case OBJECT_TYPE_STRUCTURE:
		if (custom_message->Read_Message(2) == FOCUS_SPECIFC_OBJECT)
		{
			Coordinate grid_pos = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
			if (custom_message->Read_Message(4) == grid_pos.x && custom_message->Read_Message(5) == grid_pos.y)
			{
				if (custom_message->Read_Message(3) == STAT_UPDATE)
				{
					Update_Stat(custom_message->Read_Message(6), custom_message->Read_Message(7));
				}
				else if (custom_message->Read_Message(3) == STAT_ADJUST)
				{
					Adjust_Stat(custom_message->Read_Message(6), custom_message->Read_Message(7));
				}
			}
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	}
}

void AI_Stats_Component::Handle_Stat_Above_Threshold(int stat_name)
{
	switch (stat_name)
	{
	case STAT_ENTITY_HUNGER:
		Dynamic_Stat * hunger_stat = object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER);
		if (hunger_stat->current_associated_job == NULL || hunger_stat->current_associated_job->Return_Is_Init() == false)
		{
			if (object_locator->Return_AI_Item_Pointer()->Return_Amount_Of_Item_Or_Type_In_Inventory({}, "Food") > 0)
			{
				hunger_stat->current_associated_job = Create_Eat_Food_Job(object_locator->Return_AI_Item_Pointer()->Return_First_Inventory_Slot_With_Item_Or_Item_Type(0, "Food"));
			}
			else
			{
				hunger_stat->current_associated_job = Create_Find_Food_Job();
			}
		}
		break;
	}
}

void AI_Stats_Component::Increment_Dynamic_Stats()
{
	for (int i = 0; i < object_stats.entity_stats.current_num_entity_stats; i++)
	{
		if (update_cycle % object_stats.entity_stats.dynamic_stat_array[i].update_cycle == 0)
		{
			object_stats.entity_stats.dynamic_stat_array[i].stat_number += object_stats.entity_stats.dynamic_stat_array[i].auto_increment;

			if (object_stats.entity_stats.dynamic_stat_array[i].stat_number > object_stats.entity_stats.dynamic_stat_array[i].threshold_level_yellow)
			{
				Handle_Stat_Above_Threshold(object_stats.entity_stats.dynamic_stat_array[i].stat_global_name);
			}
		}
	}
}

void AI_Stats_Component::Adjust_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_BUILT_LEVEL:
		object_stats.structure_stats.built_level += new_value;
		break;
	case STAT_OBJECT_HEALTH:
		object_stats.object_health += new_value;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		object_stats.structure_stats.oxygen_level += new_value;
		break;
	case STAT_STRUCTURE_POWERED:
		object_stats.structure_stats.powered += new_value;
		break;
	case STAT_ENTITY_HUNGER:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER)->stat_number += new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_TIREDNESS)->stat_number += new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_OXYGEN)->stat_number += new_value;
		break;
	case STAT_ENTITY_FEAR:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_FEAR)->stat_number += new_value;
		break;
	case STAT_ENTITY_SPEED:
		object_stats.entity_stats.speed += new_value;
		break;
	case STAT_ENTITY_ENNUI:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_ENNUI)->stat_number += new_value;
		break;
	case STAT_PROJECTILE_RANGE:
		object_stats.projectile_stats.remaining_lifespan += new_value;
		break;
	}
}

int AI_Stats_Component::Return_Stat_Value(int stat_name)
{
	switch (stat_name)
	{
	case STAT_UNIQ_ID:
		return uniq_id;
		break;
	case STAT_OBJECT_FACTION:
		return object_stats.object_faction;
		break;
	case STAT_STRUCTURE_BUILT_LEVEL:
		return object_stats.structure_stats.built_level;
		break;
	case STAT_OBJECT_HEALTH:
		return object_stats.object_health;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		return object_stats.structure_stats.oxygen_level;
		break;
	case STAT_STRUCTURE_POWERED:
		return object_stats.structure_stats.powered;
		break;
	case STAT_STRUCTURE_IMPASSABLE:
		return object_stats.structure_stats.impassable;
		break;
	case STAT_ENTITY_HUNGER:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER)->stat_number;
		break;
	case STAT_ENTITY_TIREDNESS:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_TIREDNESS)->stat_number;
		break;
	case STAT_ENTITY_OXYGEN:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_OXYGEN)->stat_number;
		break;
	case STAT_ENTITY_FEAR:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_FEAR)->stat_number;
		break;
	case STAT_ENTITY_SPEED:
		return object_stats.entity_stats.speed;
		break;
	case STAT_ENTITY_ENNUI:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_ENNUI)->stat_number;
		break;
	case STAT_PROJECTILE_RANGE:
		return object_stats.projectile_stats.remaining_lifespan;
		break;
	case STAT_CURRENT_X_TILE:
		return object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord().x;
		break;
	case STAT_CURRENT_Y_TILE:
		return object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord().y;
		break;
	}
}

int AI_Stats_Component::Return_Max_Stat_Value(int stat_name)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_BUILT_LEVEL:
		return object_stats.structure_stats.max_build_level;
		break;
	case STAT_OBJECT_HEALTH:
		return object_stats.object_max_health;
		break;
	case STAT_ENTITY_HUNGER:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER)->stat_limit;
		break;
	case STAT_ENTITY_TIREDNESS:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_TIREDNESS)->stat_limit;
		break;
	case STAT_ENTITY_OXYGEN:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_OXYGEN)->stat_limit;
		break;
	case STAT_ENTITY_FEAR:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_FEAR)->stat_limit;
		break;
	case STAT_ENTITY_ENNUI:
		return object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_ENNUI)->stat_limit;
		break;
	}
}

void AI_Stats_Component::Update_Max_Stat_Value(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_BUILT_LEVEL:
		object_stats.structure_stats.max_build_level = new_value;
		break;
	case STAT_OBJECT_HEALTH:
		object_stats.object_max_health = new_value;
		break;
	case STAT_ENTITY_HUNGER:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_HUNGER)->stat_limit = new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_TIREDNESS)->stat_limit = new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_OXYGEN)->stat_limit = new_value;
		break;
	case STAT_ENTITY_FEAR:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_FEAR)->stat_limit = new_value;
		break;
	case STAT_ENTITY_ENNUI:
		object_stats.entity_stats.Return_Stat_Pointer(STAT_ENTITY_ENNUI)->stat_limit = new_value;
		break;
	}
}

int AI_Stats_Component::Return_Template_ID()
{
	return object_template_id;
}

int AI_Stats_Component::Return_Object_Type()
{
	return object_type;
}

void AI_Stats_Component::Check_For_Messages()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		switch (service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(0))
		{
		case MESSAGE_TYPE_STAT_UPDATE_REQUEST:
			Handle_Stat_Message(&service_locator->get_MB_Pointer()->Custom_Message_Array[i]);
			break;
		case MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION:
			Update_Stat(STAT_STRUCTURE_OXYGEN_LEVEL, 0);
			break;
		}
	}
}

Job* AI_Stats_Component::Create_Find_Food_Job()
{
	vector<Item_Location*> food_locations = service_locator->get_Scene_Graph()->Return_Vector_Of_Item_Locations("Food", 0, 0, 0);
	Item_Location* ideal_location = food_locations[service_locator->get_Scene_Graph()->Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(food_locations)];
	Blueprint food_item;
	food_item.blueprint_items[0] = *ideal_location->inventory_slot;
	food_item.blueprint_items[0].item_quantity = 1;
	food_item.Num_Items_In_Blueprint = 1;
	return service_locator->get_Scene_Graph()->Job_Create_Transport_Blueprint_Items_From_Object_To_Requestee(ideal_location->associated_object, object_locator->Return_Object_Pointer(), food_item, 0, object_locator->Return_Object_Pointer());
}

Job* AI_Stats_Component::Create_Eat_Food_Job(Item_Slot* food_item)
{
	Object* eating_object = object_locator->Return_Object_Pointer();
	object_locator->Return_AI_Item_Pointer()->Remove_Item_From_Inventory(food_item->slot_item.item_template_id, 1);

	return service_locator->get_Scene_Graph()->Job_Create_Entity_Go_Change_Object_Stat(eating_object, 0, eating_object, OBJECT_TYPE_ENTITY, STAT_ENTITY_HUNGER, LOWER_THAN_OR_EQUAL_TO, -1, 0);
}

bool AI_Stats_Component::Check_For_Death()
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		if (object_stats.object_health <= 0)
		{
			if (object_locator->Return_AI_Item_Pointer()->Return_Num_Occupied_Inventory_Slots() > 0)
			{
				service_locator->get_Scene_Graph()->Create_Container(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object_locator->Return_AI_Item_Pointer()->Return_Inventory_Slot_As_Pointer(0), object_locator->Return_AI_Item_Pointer()->Return_Num_Inventory_Slots(), 1);
				
				// Have to clear all inventory here so it is officially removed from the manifest
				object_locator->Return_AI_Item_Pointer()->Clear_All_Inventory();
			}

			service_locator->get_Scene_Graph()->Delete_Structure_Update_Tile_Map_Send_Message(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), service_locator->get_Game_Library()->Fetch_Structure_Template(object_template_id)->tile_layer);
			service_locator->get_Scene_Graph()->Create_Projectile(object_locator->Return_Object_Pointer(), object_locator->Return_Object_Pointer(), 3, 0);
			return true;
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	case OBJECT_TYPE_PROJECTILE:
		if (Return_Stat_Value(STAT_PROJECTILE_RANGE) <= 0)
		{
			service_locator->get_Scene_Graph()->Delete_Object(OBJECT_TYPE_PROJECTILE, object_locator->Return_Object_Pointer()->Get_Array_Index());
			return true;
		}
		break;
	case OBJECT_TYPE_CONTAINER:
		if (object_locator->Return_AI_Item_Pointer()->Return_Num_Occupied_Inventory_Slots() == 0)
		{
			// Do not have to clear inventory here, since theoretically the Container is only destroyed if it's inventory is already clear
			service_locator->get_Scene_Graph()->Delete_Object(OBJECT_TYPE_CONTAINER, object_locator->Return_Object_Pointer()->Get_Array_Index());
			return true;
		}
		break;
	case OBJECT_TYPE_SCAFFOLD:
		if (object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_BUILT_LEVEL) >= service_locator->get_Game_Library()->Fetch_Structure_Template(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->max_built_level)
		{	
			// Have to clear all inventory here so it is officially removed from the manifest
			object_locator->Return_AI_Item_Pointer()->Clear_All_Inventory();
			service_locator->get_Scene_Graph()->Create_New_Structure(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object_template_id, object_stats.object_faction, true);
			service_locator->get_Scene_Graph()->Delete_Object(OBJECT_TYPE_SCAFFOLD, object_array_locator);
		}
		break;
	}
	return false;
}