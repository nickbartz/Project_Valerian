#pragma once
#include<Global_Constants.h>
#include<Render_Component.h>
#include<Service_Locator.h>
#include<AI_Job_Component.h>

class SDL_Renderer;
struct Structure_Template;
struct Adjacent_Structure_Array;
class AI_Stats_Component;
class AI_Movement_Component;
class AI_Rel_Component;
class AI_Item_Component;
class Coordinate;
class Item_Slot;

class Object
{
public:
	Object(int array_index =0, SDL_Rect location = { 0,0,0,0 }, Global_Service_Locator* service_locator = NULL);

	//Update Functions
	void Update();
	void Collect_Bus_Messages();

	// Accessors
	int Get_Array_Index();
	int Get_Assigned_Flag();
	int Get_Structure_Type();
	bool Is_Structure_Inaccessible(int faction);
	int Return_Object_Array_Index();
	Coordinate get_coordinate();
	void* Return_Object_Component_Pointer(int component_type);
	int Get_Uniq_ID();

	// Draw functions
	void Draw();
	void Draw(SDL_Rect overwrite_pos);

	// Init Functions
	void Init_Structure_From_Template(Structure_Template object_config, Adjacent_Structure_Array neighbors, int faction);
	void Init_Entity_From_Template(Entity_Template object_config, int faction);
	void Init_Projectile_From_Template(Projectile_Template* projectile_config, SDL_Point target, int faction);
	void Init_Container_From_Inventory(Item_Slot inventory_pointer[], int num_inventory_items);

	void Set_Assigned_Flag(int assigned_flag); 	// Unassigning an object means we're setting its type to unassigned so that it can be overwritten in whatever array it is in

	void free();
	
private:
	Global_Service_Locator* service_locator;
	Object_Service_Locator object_service_locator;

	int uniq_id;
	SDL_Rect temp_location;

	int SG_object_array_index;

	// An unassigned object can be overwritten by a new object in an array
	int assigned_flag = OBJECT_UNASSIGNED;

	// Components	
	Render_Component * render_component = NULL;
	AI_Stats_Component* AI_Stats = NULL;
	AI_Job_Component* AI_Job = NULL;
	AI_Movement_Component* AI_Movement = NULL;
	AI_Rel_Component* AI_Relationship = NULL;
	AI_Item_Component* AI_Items = NULL;

};

