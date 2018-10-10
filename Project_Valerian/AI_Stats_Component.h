#pragma once
#include<Game_Library.h>
#include<Service_Locator.h>

class Coordinate;
class Custom_Message;

struct Entity_Stats
{
	int entity_health = 100;
	int entity_max_health = 100;
	int hunger = 0;
	int tiredness = 0;
	int oxygen = 100;
	int fear = 0;
	int speed = 3;
	int ennui = 0;
	string entity_last_name = "Tony";
	string entity_first_name = "Stark";
};

struct Structure_Stats
{
	int built_level = 0;
	int structure_health = 100;
	int structure_max_health = 100;
	int oxygen_level = 0;
	int powered = 0;
	int impassable = 0;
	int door_open = 0;
};

class AI_Stats_Component
{
public:
	AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template structure_template, Structure_Stats structure_stats); // initialize a saved structure
	AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template entity_template); // initialize a new entity from template
	AI_Stats_Component(int object_array_index, Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template entity_template, Entity_Stats entity_stats); // initialize a saved entity

	int Get_Structure_Name();
	int Get_Structure_Type();

	string Get_Entity_Name();

	void Update_Stat(int stat_name, int new_value );
	void Adjust_Stat(int stat_name, int new_value);
	int Return_Stat_Value(int stat_name);
	int Return_Object_Type();

	void Update();

	void Check_For_Messages();

private:

	void Assign_Uniq_IDs(int object_array_index);

	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	Structure_Template structure_template;
	Structure_Stats structure_stats;
	Entity_Template entity_template;
	Entity_Stats entity_stats;

	int object_type;
	int uniq_id;
	int object_array_locator;

	void Handle_Stat_Message(Custom_Message* array_message);

};