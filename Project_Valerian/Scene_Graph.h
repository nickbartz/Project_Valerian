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

class Global_Service_Locator;
class Item_Slot;

class Scene_Graph
{
public:
	Scene_Graph(Global_Service_Locator* service_locator);

	void Update();
	void Collect_Bus_Messages();
	void Draw();
	void Draw_Background();
	void Update_Tile_Map(Coordinate grid_point, int tile_layer, Object* structure);

	// Create Objects
	bool Check_Tile_Placement(Coordinate grid_point, Structure_Template structure);
	void Create_Background();
	void Create_New_Structure(Coordinate grid_point, Structure_Template* structure, int faction, bool update_message = true);
	void Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset, int faction);

	// Delete Objects
	void Delete_Structure(Coordinate grid_point, int tile_layer);
	void Delete_Structure_Highest_Layer(Coordinate grid_point);
	void Delete_Projectile(int projectile_array_num);

	// Create Entities
	void Create_Entity(Coordinate grid_point, Entity_Template entity, int faction);
	
	// Create Projectiles
	void Create_Projectile(Object* firing_object, Object* target_object, int projectile_id, int faction);
	void Create_Laser_Between_Two_Points(Object* firing_object, Object* target_object, int projectile_id);

	// Create Container
	void Create_Container(Coordinate grid_point, Item_Slot inventory_array[], int num_items);
	bool Check_Container_Placement(Coordinate grid_point);

	// Accessors
	Adjacent_Structure_Array Return_Neighboring_Tiles(Coordinate grid_point);
	int Return_Current_Structure_Count();
	Object* Return_Object_At_Coord(int coord_x, int coord_y);
	
	Object* Return_Object_By_Type_And_Array_Num(int object_type, int array_num);
	Object* Return_Structure_By_Array_Num(int array_num);
	Object* Return_Entity_By_Array_Num(int array_num);

	// Queries
	int Check_Simple_Distance_To_Object(Object* object_a, Object* object_b);
	Coordinate Return_Nearest_Accessible_Coordinate(Coordinate origin, Coordinate destination, int requesting_faction);
	void Return_Tiles_Without_Leaks(Coordinate start_tile, vector<Coordinate> &tiles_to_oxygenate, map<Coordinate, bool> &checked_tiles, bool &is_leak);
	bool Tile_Has_Leak(Coordinate tile);
	bool Tile_Is_Wall_Or_Closed_Door(Coordinate tile);
	bool Tile_Is_Inaccessible(Coordinate tile, int requesting_faction);

	void free();
private:
	Global_Service_Locator * service_locator;

	// Structure for the background
	struct Background_Object
	{
		int x;
		int y;
		int depth;
		int type;
	};

	// Background Objects
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

	int current_num_jobs = 0;
	Job current_job_array[WORLD_MAX_NUM_JOBS];
};

