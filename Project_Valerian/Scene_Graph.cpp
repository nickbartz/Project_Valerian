#include<Scene_Graph.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<iostream>
#include<Game_Library.h>
#include<Coordinate.h>

Scene_Graph::Scene_Graph(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Scene_Graph::Update()
{
	for (int i = 0; i < current_num_structures; i++)
	{
		if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Update();
	}

	for (int i = 0; i < current_num_entities; i++)
	{
		if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Update();
	}
}

void Scene_Graph::Collect_Bus_Messages()
{
	for (int i = 0; i < current_num_structures; i++)
	{
		if (structure_array[i].Get_Assigned_Flag()== OBJECT_ASSIGNED) structure_array[i].Collect_Bus_Messages();
	}
	for (int i = 0; i < current_num_entities; i++)
	{
		if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Collect_Bus_Messages();
	}
}

void Scene_Graph::Create_Background()
{
	Adjacent_Structure_Array neighbors = {};
	
	background_star_1 = Object(0,{ 0,0,0,0 },service_locator);
	background_star_1.Init_Structure_From_Template(service_locator->get_Game_Library()->Fetch_Tile_Object_Config(1), neighbors);

	background_star_2 = Object(0,{ 0,0,0,0 }, service_locator);
	background_star_2.Init_Structure_From_Template(service_locator->get_Game_Library()->Fetch_Tile_Object_Config(2), neighbors);

	background_planetoid = Object(0,{ 0,0,0,0 }, service_locator);
	background_planetoid.Init_Structure_From_Template(service_locator->get_Game_Library()->Fetch_Tile_Object_Config(3), neighbors);

	for (int i = 0; i < WORLD_MAX_NUM_BACKGROUND_OBJECTS; i++)
	{
		background_objects[i] = { rand() % SCREEN_WIDTH - SCREEN_WIDTH / 2, rand() % SCREEN_HEIGHT - SCREEN_HEIGHT / 2, rand() % 10 + 1,rand() % 2 };
		if (background_objects[i].depth > 7)
		{
			if (rand() % 10 > 7) background_objects[i].type = 3; // Set some of the stars further back to be planetoids
		}
	}
}

void Scene_Graph::Draw_Background()
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();
	SDL_Rect pos_rect = { 0,0,0,0 };
	
	int camera_move_dampner = 10;

	for (int i = 0; i < WORLD_MAX_NUM_BACKGROUND_OBJECTS; i++)
	{	
		pos_rect = {
			SCREEN_WIDTH/2 + background_objects[i].x+ (camera.x/camera_move_dampner)/ background_objects[i].depth,
			SCREEN_HEIGHT/2 + background_objects[i].y+ (camera.y/camera_move_dampner)/ background_objects[i].depth,
			TILE_SIZE/background_objects[i].depth,
			TILE_SIZE/background_objects[i].depth
		};

		if (background_objects[i].type == 0) background_star_1.Draw(pos_rect);
		else if (background_objects[i].type == 1) background_star_2.Draw(pos_rect);
		else if (background_objects[i].type == 3) background_planetoid.Draw(pos_rect);
	}
}

void Scene_Graph::Create_New_Structure(Coordinate grid_point, Structure_Template structure_config, bool update_message)
{
	if (Check_Tile_Placement(grid_point, structure_config))
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;
		current_num_structures++;

		for (int i = 0; i < WORLD_MAX_NUM_STRUCTURES; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (structure_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				structure_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, structure_config.tile_specs.w*TILE_SIZE, structure_config.tile_specs.h*TILE_SIZE }, service_locator);

				structure_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		// Now we initialize the object we just created in the structure array with an object_config
		structure_array[array_index].Init_Structure_From_Template(structure_config, Return_Neighboring_Tiles(grid_point));

		// We update the tile map with a pointer to the new object
		Update_Tile_Map(grid_point, structure_config.tile_layer, &structure_array[array_index]);

		if (update_message)
		{
			// Finally we send a SG tile update message is sent to the main bus outlining what happened
			int update_message[8] = { MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION , OBJECT_TYPE_ANY, FOCUS_ALL,grid_point.x,grid_point.y,structure_config.tile_layer,structure_config.structure_type,structure_config.structure_id };
			service_locator->get_MB_Pointer()->Add_Custom_Message(8, update_message);
		}
	}
}

void Scene_Graph::Delete_Structure(Coordinate grid_point, int tile_layer)
{
	// Then we tell the tile map to de-reference the object at that layer at the grid point specified
	//the tile map also sets the object that was previously there to unassigned so it can be overwritten
	tile_map[grid_point].Update_Tile_Structure(NULL, tile_layer);

	// Finally we send a SG tile update message is sent to the main bus outlining what happened
	int update_message[8] = { MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION , OBJECT_TYPE_ANY, FOCUS_ALL,grid_point.x,grid_point.y,tile_layer,0,0 };
	service_locator->get_MB_Pointer()->Add_Custom_Message(8, update_message);
}

void Scene_Graph::Delete_Structure_Highest_Layer(Coordinate grid_point)
{
	if (tile_map[grid_point].Return_Tile_Type_By_Layer(TILE_LAYER_MID) != 0)
	{
		Delete_Structure(grid_point, TILE_LAYER_MID);
	}
	else if (tile_map[grid_point].Return_Tile_Type_By_Layer(TILE_LAYER_BASE) != 0)
	{
		Delete_Structure(grid_point, TILE_LAYER_BASE);
	}
}

bool Scene_Graph::Check_Tile_Placement(Coordinate grid_point, Structure_Template structure)
{
	if (tile_map[grid_point].Return_Tile_Type_By_Layer(structure.tile_layer) == STRUCTURE_ID_NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Scene_Graph::Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset)
{
	for (int p = 0; p < room_array.size(); p++)
	{
		for (int i = 0; i < room_array[p].size(); i++)
		{
			Coordinate new_coord = { i + x_tile_offset,p + y_tile_offset };
			Structure_Template object_config = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(room_array[p][i]);
			Create_New_Structure(new_coord, object_config);
		}
	}
}

void Scene_Graph::Update_Tile_Map(Coordinate grid_point, int tile_layer, Object* structure)
{
	// If there isn't a tile at those coordinates, make a tile
	if (tile_map.count(grid_point) == 0) tile_map[grid_point] = Tile(grid_point);

	// Update the pointers to the object at that layer at that gridpoint, if a pointer to an object already exists, the objects "type" will be set to "OBJECT_UNASSIGNED" so it can be overwritten
	// if the update process returns true, that means there was already an assigned structure in that slot in that tile, the current num structures is reduced by 1 to account for its removal

	if (tile_layer == TILE_LAYER_BASE) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_BASE) == true) current_num_structures--;
		
	if (tile_layer == TILE_LAYER_MID) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_MID) == true) current_num_structures--;

	if (tile_layer == TILE_LAYER_ROOF) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_ROOF) == true) current_num_structures--;

}

// Entity Creation Commands

void Scene_Graph::Create_Entity(Coordinate grid_point, Entity_Template entity)
{
	if (entity.entity_id != 0)
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;
		current_num_entities++;

		for (int i = 0; i < WORLD_MAX_NUM_ENTITIES; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (entity_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				entity_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, TILE_SIZE, TILE_SIZE }, service_locator);

				entity_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		entity_array[array_index].Init_Entity_From_Template(entity);

		// Finally we send a SG entity update message to the main bus outlining what happened
		int update_message[5] = { MESSAGE_TYPE_SG_ENTITY_CREATE , OBJECT_TYPE_ANY, FOCUS_ALL, grid_point.x,grid_point.y };
		service_locator->get_MB_Pointer()->Add_Custom_Message(5, update_message);
	}
	else
	{
		cout << "Cannot create null entity" << endl;
	}

}

void Scene_Graph::Draw()
{
	Draw_Background();

	for (int i = 0; i < current_num_structures; i++)
	{
		if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Draw();
	}

	for (int i = 0; i < current_num_entities; i++)
	{
		if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Draw();
	}
}

// Accessors 

Adjacent_Structure_Array Scene_Graph::Return_Neighboring_Tiles(Coordinate grid_point)
{
	Adjacent_Structure_Array neighbor_array = {};
	
	for (int b = 0; b < 3; b++)
	{
		for (int c = 0; c < 3; c++)
		{
			Coordinate test_coord = { grid_point.x + c - 1, grid_point.y + b - 1 };
			if (!(b == 1 && c== 1) && tile_map.count(test_coord) > 0)
			{
				for (int a = 1; a < 4; a++)
				{
					neighbor_array.asa[a-1][c][b] = tile_map[test_coord].Return_Tile_Type_By_Layer(a);
				}
			}
		}
	}

	return neighbor_array;
}

int Scene_Graph::Return_Current_Structure_Count()
{
	return current_num_structures;
}

Object* Scene_Graph::Return_Object_At_Coord(int coord_x, int coord_y)
{
	for (int i = 0; i < current_num_entities; i++)
	{
		if (entity_array[i].get_coordinate().x == coord_x && entity_array[i].get_coordinate().y == coord_y)
		{
			return &entity_array[i];
		}
	}
	return NULL;
}

void Scene_Graph::free()
{
	for (int i = 0; i < current_num_structures; i++)
	{
		structure_array[i].free();
	}

	background_star_1.free();
	background_star_2.free();
	background_planetoid.free();

	for (int i = 0; i < current_num_entities; i++)
	{
		entity_array[i].free();
	}

}

// Queries

void Scene_Graph::Return_Tiles_Without_Leaks(Coordinate start_tile, vector<Coordinate> &tiles_to_oxygenate,map<Coordinate, bool> &checked_tiles, bool &is_leak)
{
	checked_tiles[start_tile] = true;

	if (!Tile_Has_Leak(start_tile) && is_leak == false)
	{		
		tiles_to_oxygenate.push_back(start_tile);
		for (int p = -1; p < 2; p++)
		{
			for (int i = -1; i < 2; i++)
			{
				if (!(i == 0 && p == 0))
				{
					Coordinate new_start_tile = { start_tile.x + i, start_tile.y + p };
					if (checked_tiles.count(new_start_tile) == 0 && !Tile_Is_Wall_Or_Closed_Door(new_start_tile))
					{
						Return_Tiles_Without_Leaks(new_start_tile, tiles_to_oxygenate, checked_tiles, is_leak);
					}
				}
			}
		}
	}
	else
	{
		is_leak = true;
	}
}

bool Scene_Graph::Tile_Has_Leak(Coordinate tile)
{
	if (service_locator->get_Game_Library()->is_null(tile_map[tile].Return_Tile_Type_By_Layer(1)) && service_locator->get_Game_Library()->is_null(tile_map[tile].Return_Tile_Type_By_Layer(2)))
	{
		return true;
	}
	else return false;
}

bool Scene_Graph::Tile_Is_Wall_Or_Closed_Door(Coordinate tile)
{
	// will need to add in functionality for doors later
	if (service_locator->get_Game_Library()->is_wall(tile_map[tile].Return_Tile_Type_By_Layer(2))) return true;
	else return false;
}

bool Scene_Graph::Tile_Is_Inaccessible(Coordinate tile, int requesting_faction)
{
	Object* base_tile = tile_map[tile].Return_Tile_Object(TILE_LAYER_BASE);
	Object* mid_tile = tile_map[tile].Return_Tile_Object(TILE_LAYER_MID);
	
	if (base_tile != NULL && base_tile->Is_Structure_Inaccessible(requesting_faction) == true)
	{
		return true;
	}
	else if (mid_tile != NULL && mid_tile->Is_Structure_Inaccessible(requesting_faction) == true)
	{
		return true;
	}
	else return false; 
}