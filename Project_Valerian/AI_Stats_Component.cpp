#include<AI_Stats_Component.h>
#include<iostream>
#include<Render_Component.h>
#include<Coordinate.h>
#include<Message_Array.h>
#include<Message.h>
#include<AI_Movement_Component.h>
#include<AI_Job_Component.h>
#include<Scene_Graph.h>


using namespace std;

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int oType, int object_template_num)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = oType;

	switch (oType)
	{
	case OBJECT_TYPE_STRUCTURE:
		structure_template_num = oType;
		break;
	case OBJECT_TYPE_ENTITY:
		entity_template_num = oType;
		break;
	case OBJECT_TYPE_PROJECTILE:
		projectile_template_num = oType;
		projectile_stats.remaining_lifespan = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_num)->projectile_range;
		break;
	}

	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int sTemplate, Structure_Stats sStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_STRUCTURE;
	structure_template_num = sTemplate;

	structure_stats = sStats;
	Assign_Uniq_IDs(object_array_index);
}

AI_Stats_Component::AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int entity_template_num, Entity_Stats eStats)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = OBJECT_TYPE_ENTITY;
	entity_template_num = entity_template_num;

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
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_template_num)->structure_name;
}

int AI_Stats_Component::Get_Structure_Name()
{
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_template_num)->structure_id;
}

int AI_Stats_Component::Get_Structure_Type()
{
	return service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_template_num)->structure_type;
}

string AI_Stats_Component::Get_Entity_Name()
{
	return entity_stats.entity_last_name + ", " + entity_stats.entity_first_name;
}

void AI_Stats_Component::Update()
{
	// RUN OBJECT AI
	switch (object_type)
	{
	case OBJECT_TYPE_ENTITY:
		Entity_Manage_Job();
		break;
	case OBJECT_TYPE_STRUCTURE:
		break;
	case OBJECT_TYPE_PROJECTILE:
		Adjust_Stat(STAT_PROJECTILE_RANGE, -1);
		if (Return_Stat_Value(STAT_PROJECTILE_RANGE) <= 0) service_locator->get_Scene_Graph()->Delete_Projectile(object_locator->Return_Object_Pointer()->Get_Array_Index());
		break;
	}
}

void AI_Stats_Component::Update_Stat(int stat_name, int new_value)
{
	switch (stat_name)
	{
	case STAT_OBJECT_FACTION:
		object_faction = new_value;
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
	case STAT_PROJECTILE_RANGE:
		projectile_stats.remaining_lifespan = new_value;
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
	case STAT_PROJECTILE_RANGE:
		projectile_stats.remaining_lifespan += new_value;
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
		return object_faction;
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
	case STAT_PROJECTILE_RANGE:
		return projectile_stats.remaining_lifespan;
		break;
	}
}

int AI_Stats_Component::Return_Template_ID(int object_type)
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		return structure_template_num;
		break;
	case OBJECT_TYPE_ENTITY:
		return entity_template_num;
		break;
	case OBJECT_TYPE_PROJECTILE:
		return projectile_template_num;
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
		case MESSAGE_TYPE_ENTITY_JOB_REQUEST:
			if (object_type == OBJECT_TYPE_ENTITY) Assess_Job_Priority(&service_locator->get_MB_Pointer()->Custom_Message_Array[i]);
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

void AI_Stats_Component::Entity_Manage_Job()
{
	//Find_Highest_Priority_Job();

	// Check to see if the "found" job is higher or lower priority than current job
	// if there is no current job, go with highest priority available job

	// If the highest priority job is already in action, do nothing

	// If it isn't already in action - send command to AI_Job to load it as the new job

	if (next_job_length > 0)
	{
		object_locator->Return_AI_Job_Pointer()->Load_New_Job(next_job_length, next_job);
		Clear_Loaded_Job();
	}
}

void AI_Stats_Component::Load_Job_From_Message(Custom_Message* job_message)
{
	Clear_Loaded_Job();
	
	for (int i = 1; i < job_message->Get_Message_Length(); i++)
	{
		next_job[i - 1] = job_message->Read_Message(i);
	}

	next_job_length = job_message->Get_Message_Length() - 1;
}

void AI_Stats_Component::Clear_Loaded_Job()
{
	for (int i = 0; i < MAX_LENGTH_CUSTOM_MESSAGE; i++)
	{
		next_job[i] = 0;
	}

	next_job_length = 0;
}

void AI_Stats_Component::Assess_Job_Priority(Custom_Message* job_message)
{
	// TBD
	Load_Job_From_Message(job_message);
}