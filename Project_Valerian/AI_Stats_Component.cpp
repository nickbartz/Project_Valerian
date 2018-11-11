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
		break;
	case OBJECT_TYPE_ENTITY:
		object_template_id = object_template_num;
		break;
	case OBJECT_TYPE_PROJECTILE:
		object_template_id = object_template_num;
		object_stats.projectile_stats.remaining_lifespan = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_num)->projectile_range;
		break;
	case OBJECT_TYPE_CONTAINER:
		break;
	case OBJECT_TYPE_SCAFFOLD:
		object_template_id = object_template_num;
		break;
	}

	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int sTemplate, Structure_Stats sStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_STRUCTURE;
	object_template_id = sTemplate;

	object_stats.structure_stats = sStats;
	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int entity_template_num, Entity_Stats eStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_ENTITY;
	entity_template_num = entity_template_num;

	object_stats.entity_stats = eStats;
	Assign_Uniq_IDs(object_array_index);
}

void AI_Stats_Component::Assign_Uniq_IDs(int object_array_index)
{
	object_array_locator = object_array_index;
	uniq_id = rand() % 100000;
}

string AI_Stats_Component::Get_Structure_Common_Name()
{
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_template_id)->structure_name;
}

int AI_Stats_Component::Get_Structure_Name()
{
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_template_id)->structure_id;
}

int AI_Stats_Component::Get_Structure_Type()
{
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_template_id)->structure_type;
}

string AI_Stats_Component::Get_Entity_Name()
{
	return object_stats.entity_stats.entity_last_name + ", " + object_stats.entity_stats.entity_first_name;
}

void AI_Stats_Component::Update()
{
	if (!Check_For_Death())
	{
		// RUN OBJECT AI
		switch (object_type)
		{
		case OBJECT_TYPE_ENTITY:
			break;
		case OBJECT_TYPE_STRUCTURE:
			break;
		case OBJECT_TYPE_PROJECTILE:
			Adjust_Stat(STAT_PROJECTILE_RANGE, -1);
			break;
		}
	}
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
	case STAT_OBJECT_MAX_HEALTH:
		object_stats.object_max_health= new_value;
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
		object_stats.entity_stats.hunger = new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		object_stats.entity_stats.tiredness = new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		object_stats.entity_stats.oxygen = new_value;
		break;
	case STAT_ENTITY_FEAR:
		object_stats.entity_stats.fear = new_value;
		break;
	case STAT_ENTITY_SPEED:
		object_stats.entity_stats.speed = new_value;
		break;
	case STAT_ENTITY_ENNUI:
		object_stats.entity_stats.ennui = new_value;
		break;
	case STAT_PROJECTILE_RANGE:
		object_stats.projectile_stats.remaining_lifespan = new_value;
		break;
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
	case STAT_OBJECT_MAX_HEALTH:
		object_stats.object_max_health += new_value;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		object_stats.structure_stats.oxygen_level += new_value;
		break;
	case STAT_STRUCTURE_POWERED:
		object_stats.structure_stats.powered += new_value;
		break;
	case STAT_ENTITY_HUNGER:
		object_stats.entity_stats.hunger += new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		object_stats.entity_stats.tiredness += new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		object_stats.entity_stats.oxygen += new_value;
		break;
	case STAT_ENTITY_FEAR:
		object_stats.entity_stats.fear += new_value;
		break;
	case STAT_ENTITY_SPEED:
		object_stats.entity_stats.speed += new_value;
		break;
	case STAT_ENTITY_ENNUI:
		object_stats.entity_stats.ennui += new_value;
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
	case STAT_OBJECT_MAX_HEALTH:
		return object_stats.object_max_health;
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
		return object_stats.entity_stats.hunger;
		break;
	case STAT_ENTITY_TIREDNESS:
		return object_stats.entity_stats.tiredness;
		break;
	case STAT_ENTITY_OXYGEN:
		return object_stats.entity_stats.oxygen;
		break;
	case STAT_ENTITY_FEAR:
		return object_stats.entity_stats.fear;
		break;
	case STAT_ENTITY_SPEED:
		return object_stats.entity_stats.speed;
		break;
	case STAT_ENTITY_ENNUI:
		return object_stats.entity_stats.ennui;
		break;
	case STAT_PROJECTILE_RANGE:
		return object_stats.projectile_stats.remaining_lifespan;
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
			}

			service_locator->get_Scene_Graph()->Delete_Structure_Update_Tile_Map_Send_Message(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_template_id)->tile_layer);
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
			service_locator->get_Scene_Graph()->Delete_Object(OBJECT_TYPE_CONTAINER, object_locator->Return_Object_Pointer()->Get_Array_Index());
			return true;
		}
		break;
	case OBJECT_TYPE_SCAFFOLD:
		if (object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_BUILT_LEVEL) >= service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_locator->Return_AI_Stats_Pointer()->Return_Template_ID())->max_built_level)
		{
			service_locator->get_Scene_Graph()->Create_New_Structure(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object_template_id, object_stats.object_faction, true);
			service_locator->get_Scene_Graph()->Delete_Object(OBJECT_TYPE_SCAFFOLD, object_array_locator);
		}
		break;
	}
	return false;
}