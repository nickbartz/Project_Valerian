#pragma once

using namespace std;

#include<Tile.h>
#include<map>
#include<Object.h>
#include<Global_Constants.h>
#include<Coordinate.h>
#include<Adjacent_Type_Array.h>
#include<Game_Library.h>
#include<Job.h>
#include<functional>

// Structures for Storing Reference
struct Item_Location
{
	Item_Slot* inventory_slot;
	Object* associated_object;
};

class Global_Service_Locator;
struct Item_Slot;
struct Blueprint;
struct Item;

class Scene_Graph
{
public:
	Scene_Graph(Global_Service_Locator* service_locator);

	// Functions Accessed Through Main
	void Iterate_And_Function(Object object_array[], int current_num_members, int max_num_members, int major_function_type);
	void Process_Main_Function(int major_function_type);

	void Update();
	void Collect_Bus_Messages();
	void Draw();

	// Create Things
	bool Check_Tile_Placement(Coordinate grid_point, Structure_Template structure);
	void Create_Background();
	Object* Create_New_Structure(Coordinate grid_point, int structure_template_id, int faction, bool update_message = true);
	void Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset, int faction);
	void Create_New_Scaffold(Coordinate grid_point, int structure_template_id, int faction);
	void Create_Entity(Coordinate grid_point, Entity_Template entity, int faction);
	void Create_Projectile(Object* firing_object, Object* target_object, int projectile_id, int faction);
	void Create_Laser_Between_Two_Points(Object* firing_object, Object* target_object, int projectile_id);
	void Create_Container(Coordinate grid_point, Item_Slot inventory_array[], int num_items, int pickup_flag = 0);

	// Inventory Manifest Commands 
	void Send_Inventory_Manifest_Update_Message(Item_Slot* slot_pointer, Object* associated_object, int inventory_update_type);
	void Add_Inventory_Slot_To_Manifest(Item_Slot* slot_pointer, Object* associated_object);
	void Remove_Inventory_Slot_From_Manifest(Item_Slot* slot_pointer);
	int Return_Num_Item_Slots_On_Manifest();

	// Job Related Functions
	Job* Return_Job_With_Highest_Priority_Correlation(Object* requesting_object);
	Job* Add_Raw_Job_To_Job_Array(vector<Goal_Set> goal_sets, int job_id, Object* job_requestor, int public_private);
	void Check_If_Job_Can_Be_Closed(int job_array_num);

	// Job Create Functions - Low Level
	Job* Job_Create_Transport_Blueprint_Items_From_Object_To_Requestee(Object* object, Object* Requestee, Blueprint blueprint, int public_private = 1, Object* assigned_object = NULL);
	Job* Job_Create_Pickup_Container(Object* container, int public_private = 1, Object* assigned_object = NULL);
	Job* Job_Create_Mine_Asteroid(Object* asteroid, int public_private = 1, Object* assigned_object = NULL);
	Job* Job_Create_Entity_Go_Change_Object_Stat(Object* scaffold, int public_private = 1, Object* assigned_object = NULL, int object_type = OBJECT_TYPE_SCAFFOLD, int stat = STAT_STRUCTURE_BUILT_LEVEL, int comparator_function = HIGHER_THAN_OR_EQUAL_TO, int increment = 1, int desired_value = 255);
	Job* Job_Create_Local_Oxygenate(Object* object);
	Job* Job_Create_Local_Open_Door(Object* object);

	// Job Create Functions - Moderate Complexity
	bool Job_Create_Find_Blueprint_Items_And_Transport_To_Requestee(Object* requestee, Blueprint* blueprint, bool create_job, int public_private = 1, Object* assigned_object = NULL);
	Job* Job_Create_Local_Structure_Template_Job(Object* object, int job_id);

	// Delete Objects
	void Delete_Object(int object_type, int array_num);
	void Delete_Structure_Update_Tile_Map_Send_Message(Coordinate grid_point, int tile_layer);
	void Delete_Structure_Highest_Layer(Coordinate grid_point);
	void Delete_Scaffold(Coordinate grid_point);
	
	// Accessors
	Adjacent_Structure_Array Return_Neighboring_Tiles(Coordinate grid_point);

	vector<Item_Location*> Return_Vector_Of_Item_Locations(string item_type, int item_id, int structure_type, int structure_id);

	vector<Object*> Return_All_Entities_On_Tile(Coordinate tile);
	vector<Object*> Return_All_Entities_In_Radius(Coordinate tile, int radius);

	int Return_Current_Num_Jobs_In_Array();
	int Return_Current_Num_Public_Jobs_In_Array();
	int Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(vector<Item_Location*> storage_locations_with_item);
	int Return_Current_Structure_Count();

	Object* Return_Object_At_Coord(int coord_x, int coord_y);
	Object* Return_Structure_At_Coord_By_Layer(int coord_x, int coord_y, int layer);
	Object* Return_Nearest_Structure_By_Type(Object* local_object, string structure_type);
	Object* Return_Object_By_Type_And_Array_Num(int object_type, int array_num);
	Object* Return_Structure_By_Array_Num(int array_num);
	Object* Return_Entity_By_Array_Num(int array_num);

	
	// Queries
	Coordinate Return_Nearest_Accessible_Coordinate(Coordinate origin, Coordinate destination, int requesting_faction);
	int Check_Simple_Distance_To_Object(Object* object_a, Object* object_b);
	void Return_Tiles_Without_Leaks(Coordinate start_tile, vector<Coordinate> &tiles_to_oxygenate, map<Coordinate, bool> &checked_tiles, bool &is_leak);
	bool Check_If_Tile_Has_Leak(Coordinate tile);
	bool Check_If_Tile_Is_Wall_Or_Closed_Door(Coordinate tile);
	bool Check_If_Tile_Is_Inaccessible(Coordinate tile, int requesting_faction);

	void free();
private:
	Global_Service_Locator * service_locator;

	void Draw_Background();
	void Update_Tile_Map(Coordinate grid_point, int tile_layer, Object* structure);
	
	// Functions to check object placement
	bool Check_Scaffold_Placement(Coordinate grid_point, int structure_id);
	bool Check_Container_Placement(Coordinate grid_point);

	// Job-Subroutines
	bool Create_Goalsets_To_Obtain_Item_Quantity_From_Multiple_Storage_Locations(vector<Goal_Set> &goal_set_vector, Item_Slot item_slot, Object* requestee);
	Goal_Set Goalset_Create_Move_Item_From_Object_To_Object(Item item, int amount_of_item, Object* start_object, Object* finish_object);

	// Structure for the background
	struct Background_Object
	{
		int x;
		int y;
		int depth;
		int type;
	};
	Background_Object background_objects[1000];
	Object background_star_1;
	Object background_star_2;
	Object background_planetoid;

	// THE ACTUAL OBJECTS IN THE SCENE
	map <Coordinate, Tile> tile_map;

	int current_num_structures = 0;
	Object structure_array[WORLD_MAX_NUM_STRUCTURES];

	int current_num_entities = 0;
	Object entity_array[WORLD_MAX_NUM_ENTITIES];

	int current_num_projectiles = 0;
	Object projectile_array[WORLD_MAX_NUM_PROJECTILES];

	int current_num_containers = 0;
	Object container_array[WORLD_MAX_NUM_CONTAINERS];

	int current_num_scaffolds = 0;
	Object scaffold_array[WORLD_MAX_NUM_SCAFFOLDS];

	int current_num_jobs = 0;
	int current_num_public_jobs = 0;
	Job current_job_array[WORLD_MAX_NUM_JOBS];

	int curent_num_item_slots_on_manifest = 0;
	vector<Item_Location> inventory_manifest;
};

