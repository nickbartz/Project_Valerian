#pragma once

using namespace std;

#include<Tile.h>
#include<map>
#include<Object.h>
#include<Global_Constants.h>
#include<Coordinate.h>
#include<Adjacent_Type_Array.h>
#include<Game_Library.h>

class Global_Service_Locator;

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
	void Create_New_Structure(Coordinate grid_point, Structure_Template structure, bool update_message = true);
	void Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset);

	// Delete Objects
	void Delete_Structure(Coordinate grid_point, int tile_layer);
	void Delete_Structure_Highest_Layer(Coordinate grid_point);
	void Delete_Projectile(int projectile_array_num);

	// Create Entities
	void Create_Entity(Coordinate grid_point, Entity_Template entity);
	
	// Create Projectiles
	void Create_Projectile(Object* firing_object, Projectile_Template projectile_config, SDL_Point start, SDL_Point target);

	// Accessors
	Adjacent_Structure_Array Return_Neighboring_Tiles(Coordinate grid_point);
	int Return_Current_Structure_Count();
	Object* Return_Object_At_Coord(int coord_x, int coord_y);
	Object* Return_Structure_By_Array_Num(int array_num);
	Object* Return_Entity_By_Array_Num(int array_num);

	// Queries
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
};

