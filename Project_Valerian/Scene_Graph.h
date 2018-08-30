#pragma once

using namespace std;

#include<Tile.h>
#include<map>
#include<Object.h>
#include<Global_Constants.h>
#include<Coordinate.h>
#include<Adjacent_Type_Array.h>
#include<Game_Library.h>

class Service_Locator;

class Scene_Graph
{
public:
	Scene_Graph(Service_Locator* service_locator);
	void Draw();
	void Draw_Background();
	void Update_Tile_Map(Coordinate grid_point, int tile_layer, Object* structure);

	void Create_New_Structure(Coordinate grid_point, Object_Config structure);

	// Accessors
	int Return_Current_Structure_Count();

	void free();
private:
	Service_Locator * service_locator;

	void Assign_AI_Components(Object* object, Object_Config object_config);
	void Assign_Physics_Components(Object* object, Object_Config object_config);
	void Assign_Render_Components(Object* object, Object_Config object_config);
	
	
	void Create_Background();

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

