#pragma once
#include<Game_Library.h>
#include<Service_Locator.h>
#include<map>

class Coordinate;
class Custom_Message;

struct Dynamic_Stat
{
	int stat_global_name = 0;
	int stat_number = 0;
	int update_cycle = 1;
	int auto_increment = 0;
	int threshold_level_yellow = 0;
	int threshold_level_red = 0;
	int stat_limit = 0;
	Job* current_associated_job = NULL;
};

struct Entity_Stats
{
	// Stat Entity_Stats;
	int speed = 3;
	int entity_sex = ENTITY_NEUTRAL;

	Dynamic_Stat dynamic_stat_array[MAX_ENTITY_STATS];
	int current_num_entity_stats = 0;
	map<int, int> name_crosswalk;

	void Create_Dynamic_Stat(int stat_global_name, int current_value, int update_cycle, int auto_increment, int threshold_level_yellow, int threshold_level_red, int stat_limit)
	{
		Dynamic_Stat new_dynamic_stat = { stat_global_name, current_value, update_cycle, auto_increment, threshold_level_yellow, threshold_level_red, stat_limit };
		name_crosswalk.insert(pair<int, int>(stat_global_name, current_num_entity_stats));
		dynamic_stat_array[current_num_entity_stats] = new_dynamic_stat;
		current_num_entity_stats++;
	}

	Dynamic_Stat* Return_Stat_Pointer(int stat_name)
	{
		int array_index =  name_crosswalk[stat_name];
		return &dynamic_stat_array[array_index];
	}
};

struct Structure_Stats
{
	int built_level = 0;
	int max_build_level = 0;
	int oxygen_level = 0;
	int powered = 0;
	int impassable = 0;
	int door_open = 0;
};

struct Projectile_Stats
{
	int remaining_lifespan;
};

struct Object_Stats
{
	int object_faction = 0;
	int object_health = 100;
	int object_max_health = 100;
	Entity_Stats entity_stats;
	Structure_Stats structure_stats;
	Projectile_Stats projectile_stats;
	string entity_last_name = "";
	string entity_first_name = "";
};

class AI_Stats_Component
{
public:
	AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int object_type, int object_template_num); // initialize a new object
	AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int structure_template_num, Structure_Stats structure_stats); // initialize a saved structure

	int Get_Structure_Name();
	int Get_Structure_Type();
	string Get_Structure_Common_Name();
	string Get_Entity_Name();

	void Update_Stat(int stat_name, int new_value );
	void Adjust_Stat(int stat_name, int new_value);

	int Return_Template_ID();
	int Return_Stat_Value(int stat_name);
	int Return_Object_Type();

	void Update();

	void Check_For_Messages();

private:
	void Assign_Uniq_IDs(int object_array_index);

	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	string Generate_Entity_Name();

	// Theoretically not a great idea to copy the entire template to each object, maybe just use template_id and then reference as in projectile example below
	Object_Stats object_stats;
	int object_template_id = 0;
	int object_type;
	int uniq_id;
	int object_array_locator;

	void Update_Entity();
	void Increment_Dynamic_Stats();
	void Handle_Stat_Above_Threshold(int stat_name);

	void Handle_Stat_Message(Custom_Message* array_message);

	bool Check_For_Death();

	//Entity Lifecycle Functions
	Job* Create_Find_Food_Job();
	Job* Create_Eat_Food_Job(Item_Slot* food_item);

	int update_cycle = 1;

};