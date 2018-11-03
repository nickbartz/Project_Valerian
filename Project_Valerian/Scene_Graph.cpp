#include<Scene_Graph.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<iostream>
#include<Game_Library.h>
#include<Coordinate.h>
#include<AI_Rel_Component.h>
#include<algorithm>
#include<Draw_System.h>
#include<Object.h>
#include<AI_Stats_Component.h>
#include<AI_Item_Component.h>

Scene_Graph::Scene_Graph(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Scene_Graph::Update()
{
	for (int i = 0; i < current_num_structures; i++) if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Update();

	for (int i = 0; i < current_num_entities; i++) if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Update();

	for (int i = 0; i < current_num_projectiles; i++) if (projectile_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) projectile_array[i].Update();

	for (int i = 0; i < current_num_containers; i++) if (container_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) container_array[i].Update();
}

void Scene_Graph::Collect_Bus_Messages()
{
	for (int i = 0; i < current_num_structures; i++) if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Collect_Bus_Messages();

	for (int i = 0; i < current_num_entities; i++) if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Collect_Bus_Messages();

	for (int i = 0; i < current_num_projectiles; i++) if (projectile_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) projectile_array[i].Collect_Bus_Messages();
}

void Scene_Graph::Draw()
{
	// Background will only draw if there is movement on the screen, otherwise is prebaked texture
	Draw_Background();

	for (int i = 0; i < current_num_structures; i++) if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Draw();

	for (int i = 0; i < current_num_entities; i++) if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) entity_array[i].Draw();

	for (int i = 0; i < current_num_projectiles; i++) if (projectile_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) projectile_array[i].Draw();

	for (int i = 0; i < current_num_containers; i++) if (container_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) container_array[i].Draw();

}

void Scene_Graph::Create_Background()
{
	Adjacent_Structure_Array neighbors = {};
	
	background_star_1 = Object(0,{ 0,0,0,0 },service_locator);
	background_star_1.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Tile_Object_Config(1), neighbors,0);

	background_star_2 = Object(0,{ 0,0,0,0 }, service_locator);
	background_star_2.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Tile_Object_Config(2), neighbors,0);

	background_planetoid = Object(0,{ 0,0,0,0 }, service_locator);
	background_planetoid.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Tile_Object_Config(3), neighbors,0);

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
	if (service_locator->get_Draw_System_Pointer()->Return_Spritesheet_Prebaked_Status(SPRITESHEET_BACKGROUND) == false)
	{
		SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();
		SDL_Rect pos_rect = { 0,0,0,0 };

		int camera_move_dampner = 10;

		for (int i = 0; i < WORLD_MAX_NUM_BACKGROUND_OBJECTS; i++)
		{
			pos_rect = {
				SCREEN_WIDTH / 2 + background_objects[i].x + (camera.x / camera_move_dampner) / background_objects[i].depth,
				SCREEN_HEIGHT / 2 + background_objects[i].y + (camera.y / camera_move_dampner) / background_objects[i].depth,
				TILE_SIZE / background_objects[i].depth,
				TILE_SIZE / background_objects[i].depth
			};

			if (background_objects[i].type == 0) background_star_1.Draw(pos_rect);
			else if (background_objects[i].type == 1) background_star_2.Draw(pos_rect);
			else if (background_objects[i].type == 3) background_planetoid.Draw(pos_rect);
		}
	}
}

void Scene_Graph::Create_New_Structure(Coordinate grid_point, Structure_Template* structure_config, int faction, bool update_message)
{
	if (Check_Tile_Placement(grid_point, *structure_config))
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_STRUCTURES; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (structure_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				structure_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, structure_config->tile_specs.w*TILE_SIZE, structure_config->tile_specs.h*TILE_SIZE }, service_locator);

				structure_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		if (array_index >= current_num_structures) current_num_structures++;

		// Now we initialize the object we just created in the structure array with an object_config
		structure_array[array_index].Init_Structure_From_Template(*structure_config, Return_Neighboring_Tiles(grid_point),faction);
		// We update the tile map with a pointer to the new object
		Update_Tile_Map(grid_point, structure_config->tile_layer, &structure_array[array_index]);

		if (update_message)
		{
			// Finally we send a SG tile update message is sent to the main bus outlining what happened
			int update_message[8] = { MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION , OBJECT_TYPE_ANY, FOCUS_ALL,grid_point.x,grid_point.y,structure_config->tile_layer,structure_config->structure_type,structure_config->structure_id };
			service_locator->get_MB_Pointer()->Add_Custom_Message(8, update_message);
		}
	}
}

void Scene_Graph::Delete_Structure(Coordinate grid_point, int tile_layer)
{
	// Then we tell the tile map to de-reference the object at that layer at the grid point specified
	// the tile map also sets the object that was previously there to unassigned so it can be overwritten
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

void Scene_Graph::Delete_Projectile(int projectile_array_num)
{
	projectile_array[projectile_array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
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

void Scene_Graph::Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset, int faction)
{
	for (int p = 0; p < room_array.size(); p++)
	{
		for (int i = 0; i < room_array[p].size(); i++)
		{
			Coordinate new_coord = { i + x_tile_offset,p + y_tile_offset };
			Structure_Template* object_config = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(room_array[p][i]);
			Create_New_Structure(new_coord, object_config, faction);
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

void Scene_Graph::Create_Entity(Coordinate grid_point, Entity_Template entity, int faction)
{
	if (entity.entity_id != 0)
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;

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

		if (array_index >= current_num_entities) current_num_entities++;

		entity_array[array_index].Init_Entity_From_Template(entity, faction);

		// Finally we send a SG entity update message to the main bus outlining what happened
		int update_message[5] = { MESSAGE_TYPE_SG_ENTITY_CREATE , OBJECT_TYPE_ANY, FOCUS_ALL, grid_point.x,grid_point.y };
		service_locator->get_MB_Pointer()->Add_Custom_Message(5, update_message);
	}
	else
	{
		cout << "Cannot create null entity" << endl;
	}

}



// Projectile Creation Commands

void Scene_Graph::Create_Projectile(Object* firing_object, Object* target_object, int projectile_id, int faction)
{
	Projectile_Template* projectile_config = service_locator->get_Game_Library()->Fetch_Projectile_Template(projectile_id);

	int tile_offset = 0;
	if (projectile_config->is_point_not_sprite == 1) tile_offset = TILE_SIZE / 2;

	SDL_Point start = { firing_object->get_coordinate().x*TILE_SIZE + tile_offset, firing_object->get_coordinate().y*TILE_SIZE + tile_offset };
	SDL_Point target = { target_object->get_coordinate().x*TILE_SIZE + tile_offset, target_object->get_coordinate().y*TILE_SIZE + tile_offset };

	if (projectile_config->projectile_template_id != 0)
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_PROJECTILES; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (projectile_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				projectile_array[i] = Object(i, { start.x, start.y, TILE_SIZE, TILE_SIZE }, service_locator);

				projectile_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		if (array_index >= current_num_projectiles) current_num_projectiles++;
		
		projectile_array[array_index].Init_Projectile_From_Template(projectile_config, target, faction);
		
		// Finally we send a SG entity update message to the main bus outlining what happened
		AI_Stats_Component* object_stats = (AI_Stats_Component*)firing_object->Return_Object_Component_Pointer(OBJECT_COMPONENT_STATS);
		int update_message[8] = { MESSAGE_TYPE_SG_PROJECTILE_MOVEMENT , OBJECT_TYPE_ANY, FOCUS_ALL, projectile_array[array_index].get_coordinate().x, projectile_array[array_index].get_coordinate().y,object_stats->Return_Stat_Value(STAT_OBJECT_FACTION),projectile_id, array_index };
		service_locator->get_MB_Pointer()->Add_Custom_Message(8, update_message);

	}
	else
	{
		cout << "Cannot create null projectile" << endl;
	}



}

void Scene_Graph::Create_Laser_Between_Two_Points(Object* firing_object, Object* target_object, int projectile_id)
{
	Projectile_Template* mining_laser = service_locator->get_Game_Library()->Fetch_Projectile_Template(projectile_id);
	
	SDL_Point start = service_locator->get_Cursor_Pointer()->Convert_Coord_To_Screen_Pos(firing_object->get_coordinate());
	SDL_Point end = service_locator->get_Cursor_Pointer()->Convert_Coord_To_Screen_Pos(target_object-> get_coordinate());
	SDL_Color laser_color = mining_laser->projectile_color;
	laser_color.r += rand() % (254 - laser_color.r);
	laser_color.g += rand() % (254 - laser_color.g);
	laser_color.b += rand() % (254 - laser_color.b);

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x, start.y, end.x, end.y }, true, { laser_color.r,laser_color.g,laser_color.b,laser_color.a }, PRIMITIVE_TYPE_LINE);
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x-1, start.y, end.x-1, end.y }, true, { 255,255,255,255 }, PRIMITIVE_TYPE_LINE);
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x-2, start.y, end.x-2, end.y }, true, { laser_color.r,laser_color.g,laser_color.b,laser_color.a },PRIMITIVE_TYPE_LINE);
}

// Container Creation Commands

void Scene_Graph::Create_Container(Coordinate grid_point, Item_Slot inventory_array[], int num_items)
{
	if (Check_Container_Placement(grid_point))
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_CONTAINERS; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (container_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				container_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, TILE_SIZE, TILE_SIZE }, service_locator);

				container_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		if (array_index >= current_num_containers) current_num_containers++;

		// Now we initialize the object we just created in the structure array with an object_config
		container_array[array_index].Init_Container_From_Inventory(inventory_array, num_items);

	}
}

bool Scene_Graph::Check_Container_Placement(Coordinate grid_point)
{
	return true;
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

	Object* mid_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_MID);
	if (mid_tile != NULL) return mid_tile;

	Object* base_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_BASE);
	if (base_tile != NULL) return base_tile;

	return NULL;
}

Object* Scene_Graph::Return_Object_By_Type_And_Array_Num(int object_type, int array_num)
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		return &structure_array[array_num];
		break;
	case OBJECT_TYPE_ENTITY:
		return &entity_array[array_num];
		break;
	case OBJECT_TYPE_PROJECTILE:
		return &projectile_array[array_num];
		break;
	}
}

Object* Scene_Graph::Return_Structure_By_Array_Num(int array_num)
{
	return &structure_array[array_num];
}

Object* Scene_Graph::Return_Entity_By_Array_Num(int array_num)
{
	return &entity_array[array_num];
}

void Scene_Graph::free()
{
	for (int i = 0; i < current_num_structures; i++) structure_array[i].free();

	background_star_1.free();
	background_star_2.free();
	background_planetoid.free();

	for (int i = 0; i < current_num_entities; i++) entity_array[i].free();

	for (int i = 0; i < current_num_projectiles; i++) projectile_array[i].free();
}

// Queries

int Scene_Graph::Check_Simple_Distance_To_Object(Object* object_a, Object* object_b)
{
	Coordinate object_a_coord = object_a->get_coordinate();
	Coordinate object_b_coord = object_b->get_coordinate();

	int x_dist = abs(object_b_coord.x - object_a_coord.x);
	int y_dist = abs(object_b_coord.y - object_a_coord.y);

	return max(x_dist, y_dist);
}

Coordinate Scene_Graph::Return_Nearest_Accessible_Coordinate(Coordinate origin, Coordinate destination, int requesting_faction)
{
	if (origin.x == destination.x && origin.y == destination.y) return origin;
	else
	{
		int direction_x = (destination.x - origin.x);
		int direction_y = (destination.y - origin.y);

		if (direction_x > 0) direction_x = 1;
		else if (direction_x < 0) direction_x = -1;

		if (direction_y > 0) direction_y = 1;
		else if (direction_y < 0) direction_y = -1;

		Coordinate d = destination;

		while (Tile_Is_Inaccessible(d, requesting_faction))
		{
			d.x -= direction_x;
			d.y -= direction_y;
		}

		return d;
	}
}

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