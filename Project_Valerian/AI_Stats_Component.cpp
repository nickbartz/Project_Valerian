#include<AI_Stats_Component.h>
#include<iostream>
#include<Render_Component.h>
#include<Coordinate.h>
#include<Message_Array.h>


using namespace std;

AI_Stats_Component::AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate, Structure_Stats sStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_STRUCTURE;
	structure_template = sTemplate;
	structure_stats = sStats;
}

AI_Stats_Component::AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_ENTITY;
	entity_template = eTemplate;
}

AI_Stats_Component::AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate, Entity_Stats eStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_ENTITY;
	entity_template = eTemplate;
	entity_stats = eStats;
}

int AI_Stats_Component::Get_Structure_Name()
{
	return structure_template.structure_id;
}

int AI_Stats_Component::Get_Structure_Type()
{
	return structure_template.structure_type;
}

Coordinate AI_Stats_Component::Get_Structure_Coordinate()
{
	Coordinate current_coord = { structure_stats.grid_x,structure_stats.grid_y };
	return current_coord;
}

void AI_Stats_Component::Update()
{
	if (structure_template.render_component_type == RENDER_COMPONENT_ANIMATED_CLIP)
	{
		object_locator->Return_Render_Pointer()->Increment_Structure_Animation_Frame();
	}
}

void AI_Stats_Component::Update_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_GRID_X:
		structure_stats.grid_x = new_value;
		break;
	case STAT_STRUCTURE_GRID_Y:
		structure_stats.grid_y = new_value;
		break;
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

int AI_Stats_Component::Return_Stat_Value(int stat_name)
{
	switch (stat_name)
	{
	case STAT_STRUCTURE_GRID_X:
		return structure_stats.grid_x;
		break;
	case STAT_STRUCTURE_GRID_Y:
		return structure_stats.grid_y;
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
			Handle_Stat_Update_Request(&service_locator->get_MB_Pointer()->Custom_Message_Array[i]);
			break;
		}
	}
}

void AI_Stats_Component::Handle_Stat_Update_Request(Custom_Message* custom_message)
{
	switch (custom_message->Read_Message(1))
	{
	case OBJECT_TYPE_STRUCTURE:
		if (custom_message->Read_Message(2) == FOCUS_SPECIFC_OBJECT)
		{
			if (custom_message->Read_Message(3) == structure_stats.grid_x && custom_message->Read_Message(4) == structure_stats.grid_y)
			{
				Update_Stat(custom_message->Read_Message(5), custom_message->Read_Message(6));
			}
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	}
}