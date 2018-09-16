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
	string entity_last_name;
	string entity_first_name;
};

struct Structure_Stats
{
	int grid_x = 0;
	int grid_y = 0;
	int built_level = 0;
	int structure_health = 100;
	int structure_max_health = 100;
	int oxygen_level = 0;
	int powered = 0;
	int impassable = 0;
};

class AI_Stats_Component
{
public:
	AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template structure_template, Structure_Stats structure_stats); // initialize a saved structure
	AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template entity_template); // initialize a new entity from template
	AI_Stats_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template entity_template, Entity_Stats entity_stats); // initialize a saved entity

	int Get_Structure_Name();
	int Get_Structure_Type();
	Coordinate Get_Structure_Coordinate();

	void Update_Stat(int stat_name, int new_value);
	int Return_Stat_Value(int stat_name);
	int Return_Object_Type();

	void Update();

	void Check_For_Messages();

private:

	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	Structure_Template structure_template;
	Structure_Stats structure_stats;
	Entity_Template entity_template;
	Entity_Stats entity_stats;

	int object_type;

	void Handle_Stat_Update_Request(Custom_Message* array_message);

};