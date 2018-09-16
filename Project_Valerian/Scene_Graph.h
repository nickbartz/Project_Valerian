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
	void Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset);
	void Create_New_Structure(Coordinate grid_point, Structure_Template structure);
	void Create_Background();

	// Create Entities
	void Create_Entity(Coordinate grid_point, Entity_Template entity);

	// Accessors
	Adjacent_Structure_Array Return_Neighboring_Tiles(Coordinate grid_point);
	int Return_Current_Structure_Count();

	// Queries
	void Return_Tiles_Without_Leaks(Coordinate start_tile, vector<Coordinate> &tiles_to_oxygenate, map<Coordinate, bool> &checked_tiles, bool &is_leak);
	bool Tile_Has_Leak(Coordinate tile);
	bool Tile_Is_Wall_Or_Closed_Door(Coordinate tile);

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
};

