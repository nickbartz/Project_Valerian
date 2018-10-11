#include<AI_Stats_Component.h>
#include<iostream>
#include<Render_Component.h>
#include<Coordinate.h>
#include<Message_Array.h>
#include<AI_Movement_Component.h>


using namespace std;

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate, Structure_Stats sStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_STRUCTURE;
	structure_template = sTemplate;
	structure_stats = sStats;
	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_ENTITY;
	entity_template = eTemplate;
	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate, Entity_Stats eStats)
{
	service_locator = sLocator;
	object_locator = oLocator;

	entity_template = eTemplate;
	entity_stats = eStats;
	Assign_Uniq_IDs(object_array_index);
}

void AI_Stats_Component::Assign_Uniq_IDs(int object_array_index)
{
	object_array_locator = object_array_index;
	uniq_id = rand() % 100000;
}

string AI_Stats_Component::Get_Structure_Common_Name()
{
	return structure_template.structure_name;
}

int AI_Stats_Component::Get_Structure_Name()
{
	return structure_template.structure_id;
}

int AI_Stats_Component::Get_Structure_Type()
{
	return structure_template.structure_type;
}

string AI_Stats_Component::Get_Entity_Name()
{
	return entity_stats.entity_last_name + ", " + entity_stats.entity_first_name;
}

void AI_Stats_Component::Update()
{

}

void AI_Stats_Component::Update_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_BUILT_LEVEL:
		structure_stats.built_level = new_value;
		break;
	case STAT_STRUCTURE_STRUCTURE_HEALTH:
		structure_stats.structure_health = new_value;
		break;
	case STAT_STRUCTURE_STRUCTURE_MAX_HEALTH:
		structure_stats.structure_max_health = new_value;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		structure_stats.oxygen_level = new_value;
		break;
	case STAT_STRUCTURE_POWERED:
		structure_stats.powered = new_value;
		break;
	case STAT_STRUCTURE_IMPASSABLE:
		structure_stats.impassable = new_value;
		break;
	case STAT_ENTITY_HEALTH:
		entity_stats.entity_health = new_value;
		break;
	case STAT_ENTITY_MAX_HEALTH:
		entity_stats.entity_max_health = new_value;
		break;
	case STAT_ENTITY_HUNGER:
		entity_stats.hunger = new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		entity_stats.tiredness = new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		entity_stats.oxygen = new_value;
		break;
	case STAT_ENTITY_FEAR:
		entity_stats.fear = new_value;
		break;
	case STAT_ENTITY_SPEED:
		entity_stats.speed = new_value;
		break;
	case STAT_ENTITY_ENNUI:
		entity_stats.ennui = new_value;
		break;
	}
}

void AI_Stats_Component::Adjust_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_BUILT_LEVEL:
		structure_stats.built_level += new_value;
		break;
	case STAT_STRUCTURE_STRUCTURE_HEALTH:
		structure_stats.structure_health += new_value;
		break;
	case STAT_STRUCTURE_STRUCTURE_MAX_HEALTH:
		structure_stats.structure_max_health += new_value;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		structure_stats.oxygen_level += new_value;
		break;
	case STAT_STRUCTURE_POWERED:
		structure_stats.powered += new_value;
		break;
	case STAT_ENTITY_HEALTH:
		entity_stats.entity_health += new_value;
		break;
	case STAT_ENTITY_MAX_HEALTH:
		entity_stats.entity_max_health += new_value;
		break;
	case STAT_ENTITY_HUNGER:
		entity_stats.hunger += new_value;
		break;
	case STAT_ENTITY_TIREDNESS:
		entity_stats.tiredness += new_value;
		break;
	case STAT_ENTITY_OXYGEN:
		entity_stats.oxygen += new_value;
		break;
	case STAT_ENTITY_FEAR:
		entity_stats.fear += new_value;
		break;
	case STAT_ENTITY_SPEED:
		entity_stats.speed += new_value;
		break;
	case STAT_ENTITY_ENNUI:
		entity_stats.ennui += new_value;
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
	case STAT_STRUCTURE_BUILT_LEVEL:
		return structure_stats.built_level;
		break;
	case STAT_STRUCTURE_STRUCTURE_HEALTH:
		return structure_stats.structure_health;
		break;
	case STAT_STRUCTURE_STRUCTURE_MAX_HEALTH:
		return structure_stats.structure_max_health;
		break;
	case STAT_STRUCTURE_OXYGEN_LEVEL:
		return structure_stats.oxygen_level;
		break;
	case STAT_STRUCTURE_POWERED:
		return structure_stats.powered;
		break;
	case STAT_STRUCTURE_IMPASSABLE:
		return structure_stats.impassable;
		break;
	case STAT_ENTITY_HEALTH:
		return entity_stats.entity_health;
		break;
	case STAT_ENTITY_MAX_HEALTH:
		return entity_stats.entity_max_health;
		break;
	case STAT_ENTITY_HUNGER:
		return entity_stats.hunger;
		break;
	case STAT_ENTITY_TIREDNESS:
		return entity_stats.tiredness;
		break;
	case STAT_ENTITY_OXYGEN:
		return entity_stats.oxygen;
		break;
	case STAT_ENTITY_FEAR:
		return entity_stats.fear;
		break;
	case STAT_ENTITY_SPEED:
		return entity_stats.speed;
		break;
	case STAT_ENTITY_ENNUI:
		return entity_stats.ennui;
		break;
	}
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