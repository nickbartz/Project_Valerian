#pragma once

#include<Global_Constants.h>

class Global_Service_Locator;
class Object_Service_Locator;
class Object;
class Coordinate;

const int AI_REL_MAX_OBJECT_RELATIONSHIPS = 100;
const int AI_REL_MAX_PROJECTILE_RELATIONSHIPS = 20;

class AI_Rel_Component
{
public:
	AI_Rel_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator);
	
	void Check_For_Messages();
	void Update();

private:
	Global_Service_Locator * service_locator;
	Object_Service_Locator* object_locator;

	struct Object_Relationship
	{
		int object_uniq_id = 0;
		Object* object_pointer;
	};

	Object_Relationship Obj_Relationship_Array[AI_REL_MAX_OBJECT_RELATIONSHIPS];
	
	// Functions to manage basic objects added or subtracted to the object pointer array
	int current_num_object_relationships = 0;
	bool Check_If_Object_Is_Dead(int rel_array_num);
	int Add_Object_Relationship(Object* object);
	void Remove_Object_Relationship(int array_num);

	// Projectile specific functions
	struct Projectile_Relationship
	{
		int init = 0;
		int rel_array_num = -1;
		int damage_taken = 0;
		int projectile_template_num = 0;
	};
	Projectile_Relationship projectile_rel_array[AI_REL_MAX_PROJECTILE_RELATIONSHIPS];

	void Add_Projectile_Relationship(Object* projectile, int projectile_temp_num);
	void Remove_Projectile_Relationship(int projectile_array_num);
	int current_num_projectile_relationships = 0;
	void Update_Projectile_Relationships();
	int Check_Object_Range(Coordinate coord);
};