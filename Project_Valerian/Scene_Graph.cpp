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

	for (int i = 0; i < current_num_scaffolds; i++) if (scaffold_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) scaffold_array[i].Update();

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
	// Background will only re-draw from individual textures if there is movement on the screen, otherwise is a prebaked texture
	Draw_Background();

	for (int i = 0; i < current_num_structures; i++) if (structure_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) structure_array[i].Draw();

	for (int i = 0; i < current_num_scaffolds; i++) if (scaffold_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED) scaffold_array[i].Draw();

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

void Scene_Graph::Create_New_Structure(Coordinate grid_point, int structure_template_id, int faction, bool update_message)
{
	Structure_Template* structure_config = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_template_id);

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

void Scene_Graph::Delete_Structure_Update_Tile_Map_Send_Message(Coordinate grid_point, int tile_layer)
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
	if (tile_map[grid_point].Return_Tile_Type_By_Layer(TILE_LAYER_SCAFFOLD) != 0)
	{
		Delete_Scaffold(grid_point);
	}
	else if (tile_map[grid_point].Return_Tile_Type_By_Layer(TILE_LAYER_MID) != 0)
	{
		Delete_Structure_Update_Tile_Map_Send_Message(grid_point, TILE_LAYER_MID);
	}
	else if (tile_map[grid_point].Return_Tile_Type_By_Layer(TILE_LAYER_BASE) != 0)
	{
		Delete_Structure_Update_Tile_Map_Send_Message(grid_point, TILE_LAYER_BASE);
	}
}

void Scene_Graph::Delete_Object(int object_type, int array_num)
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		Delete_Structure_Update_Tile_Map_Send_Message(structure_array[array_num].get_coordinate(), service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_array[array_num].Return_Stats_Component()->Return_Template_ID())->tile_layer);
		break;
	case OBJECT_TYPE_ENTITY:
		entity_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		break;
	case OBJECT_TYPE_PROJECTILE:
		projectile_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		break;
	case OBJECT_TYPE_CONTAINER:
		container_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		break;
	case OBJECT_TYPE_SCAFFOLD:
		scaffold_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		break;
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

void Scene_Graph::Stamp_Room_From_Array(vector<vector<int>> room_array, int x_tile_offset, int y_tile_offset, int faction)
{
	for (int p = 0; p < room_array.size(); p++)
	{
		for (int i = 0; i < room_array[p].size(); i++)
		{
			Coordinate new_coord = { i + x_tile_offset,p + y_tile_offset };
			Create_New_Structure(new_coord, room_array[p][i], faction);
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

// Scaffold Creation Commands

bool Scene_Graph::Check_Scaffold_Placement(Coordinate grid_point, int structure_id)
{
	if (Check_Tile_Placement(grid_point, *service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_id))) return true;
	else return false;
}

void Scene_Graph::Create_New_Scaffold(Coordinate grid_point, int structure_template_id, int faction)
{
	if (Check_Scaffold_Placement(grid_point, structure_template_id))
	{
		Structure_Template* structure_template = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(structure_template_id);

		// First we put an object in the scaffold array to represent our new scaffold and increment the number of scaffold in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_SCAFFOLDS; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (scaffold_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				scaffold_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, structure_template->tile_specs.w*TILE_SIZE, structure_template->tile_specs.h*TILE_SIZE }, service_locator);

				scaffold_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		if (array_index >= current_num_scaffolds) current_num_scaffolds++;

		tile_map[{grid_point.x, grid_point.y}].Update_Tile_Structure(&scaffold_array[array_index], TILE_LAYER_SCAFFOLD);

		// Now we initialize the object we just created in the scaffold array with an object_config
		scaffold_array[array_index].Init_Scaffold_From_Template(structure_template_id, faction);
	}
}

void Scene_Graph::Delete_Scaffold(Coordinate grid_point)
{
	tile_map[grid_point].Update_Tile_Structure(NULL, TILE_LAYER_SCAFFOLD);
}

// Projectile Creation Commands

void Scene_Graph::Create_Projectile(Object* firing_object, Object* target_object, int projectile_id, int faction)
{
	Projectile_Template* projectile_config = service_locator->get_Game_Library()->Fetch_Projectile_Template(projectile_id);

	int tile_offset = 0;

	SDL_Point start = { firing_object->get_world_pos().x, firing_object->get_world_pos().y };
	SDL_Point target = { target_object->get_world_pos().x, target_object->get_world_pos().y };

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
		AI_Stats_Component* object_stats = firing_object->Return_Stats_Component();
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
	
	SDL_Rect start_rect = service_locator->get_Cursor_Pointer()->Convert_World_Rect_To_Screen_Rect(firing_object->get_world_pos());
	SDL_Rect end_rect = service_locator->get_Cursor_Pointer()->Convert_World_Rect_To_Screen_Rect(target_object->get_world_pos());

	SDL_Point start = { start_rect.x + start_rect.w / 2, start_rect.y + start_rect.h / 2 };
	SDL_Point end = { end_rect.x + end_rect.w / 2, end_rect.y + end_rect.h / 2 };

	SDL_Color laser_color = mining_laser->projectile_color;
	laser_color.r += rand() % (254 - laser_color.r);
	laser_color.g += rand() % (254 - laser_color.g);
	laser_color.b += rand() % (254 - laser_color.b);

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x , start.y, end.x, end.y }, true, { laser_color.r,laser_color.g,laser_color.b,laser_color.a }, PRIMITIVE_TYPE_LINE);
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x-1, start.y, end.x-1, end.y }, true, { 255,255,255,255 }, PRIMITIVE_TYPE_LINE);
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x-2, start.y, end.x-2, end.y }, true, { laser_color.r,laser_color.g,laser_color.b,laser_color.a },PRIMITIVE_TYPE_LINE);
}

// Container Creation Commands

void Scene_Graph::Create_Container(Coordinate grid_point, Item_Slot inventory_array[], int num_items, int pickup_flag)
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

		if (pickup_flag == 1)
		{
			Job_Create_Pickup_Container(&container_array[array_index]);
		}
	}
}

bool Scene_Graph::Check_Container_Placement(Coordinate grid_point)
{
	return true;
}

// Job Creation Commands

void Scene_Graph::Add_Job_To_Job_Array(Job new_job)
{
	for (int i = 0; i < WORLD_MAX_NUM_JOBS + 1; i++)
	{
		if (current_job_array[i].Return_Is_Init() == false)
		{
			current_job_array[i] = new_job;
			current_job_array[i].Set_Init(true);
			current_job_array[i].Set_Job_Array_Num(i);
			current_num_jobs++;
			break;
		}
	}
}

void Scene_Graph::Job_Create_Pickup_Container(Object* container)
{
	Object* nearest_storage = Return_Nearest_Structure_By_Type(container, "STRUCTURE_TYPE_STORAGE");

	if (nearest_storage != NULL)
	{
		Job new_container_pickup_job = *service_locator->get_Game_Library()->Fetch_Job_Template(4);
		Goal_Set new_goalset;

		int move_to_container[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_CONTAINER, container->Get_Array_Index() };
		int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_CONTAINER, container->Get_Array_Index(),1 };
		int transfer_from_container[4] = { ACTION_TRANSFER, A_AT_TAKE_ALL_INVENTORY_FROM_OBJECT, OBJECT_TYPE_CONTAINER, container->Get_Array_Index() };
		int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET,new_goalset.goal_set_uniq_id };

		new_goalset.Add_Raw_Job_Goal_to_Goalset("Go to container", move_to_container, 4);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance, 5);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Transfer Inventory From Container", transfer_from_container, 4);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Close Job", close_job, 2);

		new_container_pickup_job.Add_Goal_Set(new_goalset);

		Add_Job_To_Job_Array(new_container_pickup_job);
	}
}

void Scene_Graph::Job_Create_Mine_Asteroid(Object* asteroid)
{
	Job new_mining_job = *service_locator->get_Game_Library()->Fetch_Job_Template(3);
	Goal_Set new_goal_set;
	new_goal_set.max_num_assigned_objects = 2;

	int go_to_structure[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_STRUCTURE, asteroid->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_STRUCTURE, asteroid->Get_Array_Index(),1 };
	int stop_moving[2] = { ACTION_EDIT_INTERNAL , A_EI_STOP_MOVING };
	int fire_laser[3] = { ACTION_EDIT_EXTERNAL, A_EE_FIRE_MINING_LASER_AT_STRUCTURE, asteroid->Get_Array_Index() };
	int check_asteroid_health[8] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_OBJECT_STAT, LOWER_THAN_OR_EQUAL_TO, OBJECT_TYPE_STRUCTURE,asteroid->Get_Array_Index(), STAT_OBJECT_HEALTH, 0, 4 };
	int set_wait_timer[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, };
	int wait[2] = { ACTION_EDIT_INTERNAL, A_EI_WAIT };
	int go_back_four[3] = { ACTION_GOAL_TRAVERSAL, TRAVERSAL_GOAL_DECREMENT, 4 };
	int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET, new_goal_set.goal_set_uniq_id };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go to asteroid", go_to_structure, 4);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check distance from asteroid", check_distance, 5);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Stop moving", stop_moving, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Fire mining laser", fire_laser, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check the asteroids health", check_asteroid_health, 8);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Set wait timer", set_wait_timer, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Wait", wait, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go back four steps", go_back_four, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Close job", close_job, 2);

	new_mining_job.Add_Goal_Set(new_goal_set);
	Add_Job_To_Job_Array(new_mining_job);
}

void Scene_Graph::Job_Create_Build_Scaffold(Object* scaffold)
{
	Job new_scaffold_job = *service_locator->get_Game_Library()->Fetch_Job_Template(5);
	Goal_Set new_goal_set;
	new_goal_set.max_num_assigned_objects = 4;
	int max_built_level = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(scaffold->Return_Stats_Component()->Return_Template_ID())->max_built_level;

	int go_to_scaffold[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_SCAFFOLD, scaffold->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_SCAFFOLD, scaffold->Get_Array_Index(),1 };
	int stop_moving[2] = { ACTION_EDIT_INTERNAL , A_EI_STOP_MOVING };
	int increment_scaffold_built[6] = { ACTION_EDIT_EXTERNAL, A_EE_ADJUST_OBJECT_STAT, OBJECT_TYPE_SCAFFOLD, scaffold->Get_Array_Index(), STAT_STRUCTURE_BUILT_LEVEL, 1 };
	int check_scaffold_built_level[8] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_OBJECT_STAT, HIGHER_THAN_OR_EQUAL_TO, OBJECT_TYPE_SCAFFOLD,scaffold->Get_Array_Index(), STAT_STRUCTURE_BUILT_LEVEL,max_built_level, 4 };
	int set_wait_timer[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, };
	int wait[2] = { ACTION_EDIT_INTERNAL, A_EI_WAIT };
	int go_back_four[3] = { ACTION_GOAL_TRAVERSAL, TRAVERSAL_GOAL_DECREMENT, 4 };
	int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET,new_goal_set.goal_set_uniq_id };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go to Scaffold", go_to_scaffold, 4);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check distance from scaffold", check_distance, 5);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Stop moving", stop_moving, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Increment Scaffold Built", increment_scaffold_built, 6);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check the scaffold built level", check_scaffold_built_level, 8);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Set wait timer", set_wait_timer, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Wait", wait, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go back four steps", go_back_four, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Close job", close_job, 2);

	new_scaffold_job.Add_Goal_Set(new_goal_set);
	Add_Job_To_Job_Array(new_scaffold_job);

}

bool Scene_Graph::Job_Create_Transport_Items_For_Blueprint(Object* requestee, Blueprint* blueprint, bool create_job)
{
	Job Blueprint_Transport_Job = *service_locator->get_Game_Library()->Fetch_Job_Template(6);

	vector<Goal_Set> Goalsets_Item_Transport;

	for (int i = 0; i < blueprint->Num_Items_In_Blueprint; i++)
	{
		if (Create_Shuttle_Goalsets_From_Item_Slot(Goalsets_Item_Transport, blueprint->blueprint_items[i], requestee) == false) return false;
	}

	if (create_job && Goalsets_Item_Transport.size() > 0)
	{
		for (int i = 0; i < Goalsets_Item_Transport.size(); i++)
		{
			Blueprint_Transport_Job.Add_Goal_Set(Goalsets_Item_Transport[i]);
		}

		Add_Job_To_Job_Array(Blueprint_Transport_Job);
	}

	return true;
}

bool Scene_Graph::Create_Shuttle_Goalsets_From_Item_Slot(vector<Goal_Set> &goal_set_vector, Item_Slot item_slot, Object* requestee)
{
	Item item = item_slot.slot_item;
	int quantity_left = item_slot.item_quantity;

	vector<Object*> storage_locations_with_item = Return_Vector_Of_Storage_Locations_With_Item(item);

	while (storage_locations_with_item.size() > 0 && quantity_left > 0)
	{
		int ideal_storage_location_index = Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(storage_locations_with_item, item);

		Object* ideal_storage_location = storage_locations_with_item[ideal_storage_location_index];

		AI_Item_Component* object_item_ai_pointer = ideal_storage_location->Return_AI_Item_Component();

		int amount_of_item = min(quantity_left, object_item_ai_pointer->Return_Amount_Of_Item_In_Inventory(item));

		Goal_Set shuttle_item_goal_set = Create_Shuttle_Item_Goalset(item, amount_of_item, ideal_storage_location, requestee);
		goal_set_vector.push_back(shuttle_item_goal_set);

		storage_locations_with_item.erase(storage_locations_with_item.begin() + ideal_storage_location_index);

		quantity_left -= amount_of_item;
	}


	if (quantity_left == 0)
	{
		return true;
	}
	else if (quantity_left > 0)
	{
		return false;
	}
	else if (quantity_left < 0)
	{
		cout << "found too much of item, quantity left greater than zero, this shouldn't happen" << endl;
		return false;
	}
}

Goal_Set Scene_Graph::Create_Shuttle_Item_Goalset(Item item, int amount_of_item, Object* ideal_storage_location, Object* requestee)
{
	Goal_Set new_goalset;
	
	int move_to_storage[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_STRUCTURE, ideal_storage_location->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_STRUCTURE, ideal_storage_location->Get_Array_Index(),1 };
	int take_items[6] = { ACTION_TRANSFER, A_AT_TAKE_ITEM_FROM_OBJECT, OBJECT_TYPE_STRUCTURE, ideal_storage_location->Get_Array_Index(),item.item_template_id,amount_of_item };
	int check_for_another_goalset[2] = { ACTION_ASSESS_INTERNAL, A_AI_CHECK_JOB_FOR_MORE_GOALSETS };
	int move_to_requestee[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_SCAFFOLD, requestee->Get_Array_Index() };
	int check_distance_2[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_SCAFFOLD, requestee->Get_Array_Index(),1 };
	int give_items_to_requestee[6] = { ACTION_TRANSFER, A_AT_GIVE_ITEM_TO_OBJECT, OBJECT_TYPE_SCAFFOLD, requestee->Get_Array_Index(), item.item_template_id, amount_of_item };
	int finish_goal_set[2] = { ACTION_CLOSE_OUT_GOAL_SET, new_goalset.goal_set_uniq_id };

	new_goalset.Add_Raw_Job_Goal_to_Goalset("Move to storage", move_to_storage, 4);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance, 5);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Take Items", take_items, 6);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check job", check_for_another_goalset, 2);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Move to requestee", move_to_requestee, 4);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance_2, 5);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Give items to requestee", give_items_to_requestee, 6);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Finish goal set", finish_goal_set, 2);

	return new_goalset;
}

int Scene_Graph::Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(vector<Object*> storage_locations_with_item, Item item)
{
	// We will improve this algorithm as it becomes necessary for entities not to do stupid shit

	return 0;
}

Job* Scene_Graph::Return_Job_With_Highest_Priority_Correlation(int dummy_variable)
{
	if (current_num_jobs > 0)
	{
		for (int i = 0; i < WORLD_MAX_NUM_JOBS; i++)
		{
			if (current_job_array[i].Return_Is_Init() == true && current_job_array[i].Get_Num_Unassigned_Goalsets() > 0)
			{
				return &current_job_array[i];
			}
		}
	}

	return NULL;
}

void Scene_Graph::Check_If_Job_Can_Be_Closed(int job_array_num)
{
	for(int i = 0; i < WORLD_MAX_NUM_JOBS; i++)
	{
		if (current_job_array[i].Return_Job_Array_Num() == job_array_num && current_job_array[i].Return_Is_Init() == true)
		{
			current_job_array[i].Set_Init(false);
			current_num_jobs--;
		}
	}
}

// Accessors 

vector<Object*> Scene_Graph::Return_Vector_Of_Storage_Locations_With_Item(Item item)
{
	vector<Object*> storage_locations_with_item;
	
	for (int i = 0; i < current_num_structures; i++)
	{
		if (structure_array[i].Return_Stats_Component()->Get_Structure_Type() == service_locator->get_Game_Library()->Fetch_Structure_Type_ID_From_Name("STRUCTURE_TYPE_STORAGE"))
		{
			if (structure_array[i].Return_AI_Item_Component()->Return_Amount_Of_Item_In_Inventory(item) > 0)
			{
				storage_locations_with_item.push_back(&structure_array[i]);
			}
		}
	}

	return storage_locations_with_item;
}

int Scene_Graph::Return_Current_Num_Jobs_In_Array()
{
	return current_num_jobs;
}

Object* Scene_Graph::Return_Nearest_Structure_By_Type(Object* local_object, string structure_type)
{
	Coordinate anchor_coord = local_object->get_coordinate();
	Object* temp_object = NULL;
	
	for (int i = 0; i < current_num_structures; i++)
	{
		AI_Stats_Component* object_stats = structure_array[i].Return_Stats_Component();

		if (object_stats->Get_Structure_Type() == service_locator->get_Game_Library()->Fetch_Structure_Type_ID_From_Name(structure_type))
		{
			if (temp_object == NULL)
			{
				temp_object = &structure_array[i];
			}
			else
			{
				int existing_sum = abs(temp_object->get_coordinate().x - anchor_coord.x) + abs(temp_object->get_coordinate().y - anchor_coord.y);
				int new_sum = abs(structure_array[i].get_coordinate().x - anchor_coord.x) + abs(structure_array[i].get_coordinate().x - anchor_coord.y);

				if (new_sum < existing_sum)
				{
					temp_object = &structure_array[i];
				}
			}
		}
	}

	return temp_object;
}

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
		if (entity_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED && entity_array[i].get_coordinate().x == coord_x && entity_array[i].get_coordinate().y == coord_y)
		{
			return &entity_array[i];
		}
	}

	for (int i = 0; i < current_num_scaffolds; i++)
	{
		if (scaffold_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED && scaffold_array[i].get_coordinate().x == coord_x && scaffold_array[i].get_coordinate().y == coord_y)
		{
			return &scaffold_array[i];
		}
	}

	for (int i = 0; i < current_num_containers; i++)
	{
		if (container_array[i].Get_Assigned_Flag() == OBJECT_ASSIGNED && container_array[i].get_coordinate().x == coord_x && container_array[i].get_coordinate().y == coord_y)
		{
			return &container_array[i];
		}
	}

	Object* mid_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_MID);
	if (mid_tile != NULL) return mid_tile;

	Object* base_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_BASE);
	if (base_tile != NULL) return base_tile;

	return NULL;
}

Object* Scene_Graph::Return_Structure_At_Coord_By_Layer(int coord_x, int coord_y, int layer)
{
	switch (layer)
	{
	case TILE_LAYER_BASE:
		{Object * base_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_BASE);
		if (base_tile != NULL) return base_tile; }
		break;
	case TILE_LAYER_MID:
		{Object * mid_tile = tile_map[{coord_x, coord_y}].Return_Tile_Object(TILE_LAYER_MID);
		if (mid_tile != NULL) return mid_tile; }
		break;
	}

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
	case OBJECT_TYPE_CONTAINER:
		return &container_array[array_num];
		break;
	case OBJECT_TYPE_SCAFFOLD:
		return &scaffold_array[array_num];
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
	if (origin.x == destination.x && origin.y == destination.y)
	{
		return origin;
	}
	else
	{
		Coordinate delta[] = { {-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0} };

		int direction_x = (destination.x - origin.x);
		int direction_y = (destination.y - origin.y);

		int abs_direction_x;
		int abs_direction_y;

		if (direction_x == 0) abs_direction_x = 0;
		else abs_direction_x = direction_x / abs(direction_x);

		if (direction_y == 0) abs_direction_y = 0;
		else abs_direction_y = direction_y / abs(direction_y);

		int origin_index;

		for (int i = 0; i < 9; i++)
		{
			if (delta[i].x == abs_direction_x && delta[i].y == abs_direction_y) origin_index = i;
		}

		int delta_index = origin_index;

		Coordinate d = { destination.x + delta[delta_index].x, destination.y + delta[delta_index].y };


		while (Tile_Is_Inaccessible(d, requesting_faction))
		{
			delta_index += 1;
			if (delta_index > 7) delta_index = 0;
			if (delta_index == origin_index)
			{
				return origin;
			}
			d = { destination.x + delta[delta_index].x, destination.y + delta[delta_index].y };
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
	Object* scaffold_tile = tile_map[tile].Return_Tile_Object(TILE_LAYER_SCAFFOLD);
	Object* base_tile = tile_map[tile].Return_Tile_Object(TILE_LAYER_BASE);
	Object* mid_tile = tile_map[tile].Return_Tile_Object(TILE_LAYER_MID);
	
	if (scaffold_tile != NULL && scaffold_tile->Is_Structure_Inaccessible(requesting_faction) == true)
	{
		return true;
	}
	else if (base_tile != NULL && base_tile->Is_Structure_Inaccessible(requesting_faction) == true)
	{
		return true;
	}
	else if (mid_tile != NULL && mid_tile->Is_Structure_Inaccessible(requesting_faction) == true)
	{
		return true;
	}
	else return false; 
}