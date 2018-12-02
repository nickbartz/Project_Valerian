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
#include<UI.h>

Scene_Graph::Scene_Graph(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Scene_Graph::Add_Inventory_Slot_To_Manifest(Item_Slot* slot_pointer, Object* associated_object)
{
	inventory_manifest.push_back({ slot_pointer, associated_object });
	curent_num_item_slots_on_manifest++;
	Send_Inventory_Manifest_Update_Message(slot_pointer, associated_object, 1);
}

Job* Scene_Graph::Add_Raw_Job_To_Job_Array(vector<Goal_Set> goal_sets, int job_id, Object* job_requestor, int public_private)
{
	Job new_job = *service_locator->get_Game_Library()->Fetch_Job_Template(job_id);
	new_job.Set_Public_Private_Access(public_private);
	new_job.Set_Job_Requester(job_requestor);
	
	for (int i = 0; i < goal_sets.size(); i++)
	{
		new_job.Add_Goal_Set(goal_sets[i]);
	}

	for (int i = 0; i < WORLD_MAX_NUM_JOBS + 1; i++)
	{
		if (current_job_array[i].Return_Is_Init() == false)
		{
			current_job_array[i] = new_job;
			current_job_array[i].Set_Init(true);
			current_job_array[i].Set_Job_Array_Num(i);
			current_num_jobs++;
			if (public_private == 1) current_num_public_jobs++;

			// Send a messsage to the UI console about the job that was just created
			service_locator->get_UI_pointer()->Push_Message_To_Console("Creating New Job: " + current_job_array[i].Return_Job_String_Name());

			return &current_job_array[i];
		}
	}

	cout << "couldn't add job to array" << endl;
	return NULL;
}

int Scene_Graph::Check_Simple_Distance_To_Object(Object* object_a, Object* object_b)
{
	Coordinate object_a_coord = object_a->get_coordinate();
	Coordinate object_b_coord = object_b->get_coordinate();

	int x_dist = abs(object_b_coord.x - object_a_coord.x);
	int y_dist = abs(object_b_coord.y - object_a_coord.y);

	return (x_dist+y_dist);
}

bool Scene_Graph::Check_If_Tile_Has_Leak(Coordinate tile)
{
	if (service_locator->get_Game_Library()->is_null(tile_map[tile].Return_Tile_Type_By_Layer(1)) && service_locator->get_Game_Library()->is_null(tile_map[tile].Return_Tile_Type_By_Layer(2)))
	{
		return true;
	}
	else return false;
}

bool Scene_Graph::Check_If_Tile_Is_Wall_Or_Closed_Door(Coordinate tile)
{
	// will need to add in functionality for doors later
	if (service_locator->get_Game_Library()->is_wall(tile_map[tile].Return_Tile_Type_By_Layer(2))) return true;
	else return false;
}

bool Scene_Graph::Check_If_Tile_Is_Inaccessible(Coordinate tile, int requesting_faction)
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
		if (mid_tile->Return_Stats_Component()->Get_Structure_Type() == service_locator->get_Game_Library()->Get_Structure_Type_Code_From_Structure_Type_String("STRUCTURE_TYPE_DOOR")) cout << "found inaccessible door" << endl;
		return true;
	}
	else return false;
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

bool Scene_Graph::Check_Scaffold_Placement(Coordinate grid_point, int structure_id)
{
	if (Check_Tile_Placement(grid_point, *service_locator->get_Game_Library()->Fetch_Structure_Template(structure_id))) return true;
	else return false;
}

void Scene_Graph::Check_If_Job_Can_Be_Closed(int job_array_num)
{
	for (int i = 0; i < WORLD_MAX_NUM_JOBS; i++)
	{
		if (current_job_array[i].Return_Job_Array_Num() == job_array_num && current_job_array[i].Return_Is_Init() == true)
		{
			current_job_array[i].Set_Init(false);
			current_num_jobs--;
			if (current_job_array[i].Return_Job_Public_Private() == 1) current_num_public_jobs--;
		}
	}
}

bool Scene_Graph::Create_Goalsets_To_Obtain_Item_Quantity_From_Multiple_Storage_Locations(vector<Goal_Set> &goal_set_vector, Item_Slot item_slot, Object* requestee)
{
	Item item = item_slot.slot_item;
	int quantity_left = item_slot.item_quantity;

	vector<Item_Location*> storage_locations_with_item = Return_Vector_Of_Item_Locations("",item.item_template_id,service_locator->get_Game_Library()->Get_Structure_Type_Code_From_Structure_Type_String("STRUCTURE_TYPE_STORAGE"),0);

	while (storage_locations_with_item.size() > 0 && quantity_left > 0)
	{
		int ideal_storage_location_index = Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(storage_locations_with_item);

		Object* ideal_storage_location = storage_locations_with_item[ideal_storage_location_index]->associated_object;

		AI_Item_Component* object_item_ai_pointer = ideal_storage_location->Return_AI_Item_Component();

		int amount_of_item = min(quantity_left, object_item_ai_pointer->Return_Amount_Of_Item_Or_Type_In_Inventory(item));

		Goal_Set shuttle_item_goal_set = Goalset_Create_Move_Item_From_Object_To_Object(item, amount_of_item, ideal_storage_location, requestee);
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

bool Scene_Graph::Check_Container_Placement(Coordinate grid_point)
{
	return true;
}

void Scene_Graph::Collect_Bus_Messages()
{
	Process_Main_Function(OBJECT_FUNCTION_CHECK_FOR_MESSAGES);
}

void Scene_Graph::Create_Background()
{
	Adjacent_Structure_Array neighbors = {};

	background_star_1 = Object(0, { 0,0,0,0 }, service_locator);
	background_star_1.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Structure_Template(1), neighbors, 0);

	background_star_2 = Object(0, { 0,0,0,0 }, service_locator);
	background_star_2.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Structure_Template(2), neighbors, 0);

	background_planetoid = Object(0, { 0,0,0,0 }, service_locator);
	background_planetoid.Init_Structure_From_Template(*service_locator->get_Game_Library()->Fetch_Structure_Template(3), neighbors, 0);

	for (int i = 0; i < WORLD_MAX_NUM_BACKGROUND_OBJECTS; i++)
	{
		background_objects[i] = { rand() % SCREEN_WIDTH - SCREEN_WIDTH / 2, rand() % SCREEN_HEIGHT - SCREEN_HEIGHT / 2, rand() % 10 + 1,rand() % 2 };
		if (background_objects[i].depth > 7)
		{
			if (rand() % 10 > 7) background_objects[i].type = 3; // Set some of the stars further back to be planetoids
		}
	}
}

void Scene_Graph::Create_New_Structure(Coordinate grid_point, int structure_template_id, int faction, bool update_message)
{
	Structure_Template* structure_config = service_locator->get_Game_Library()->Fetch_Structure_Template(structure_template_id);

	if (Check_Tile_Placement(grid_point, *structure_config))
	{
		// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_STRUCTURES; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (structure_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				structure_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, structure_config->tile_specs.w*TILE_SIZE, TILE_SIZE }, service_locator);

				structure_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		current_num_structures++;

		// Now we initialize the object we just created in the structure array with an object_config
		structure_array[array_index].Init_Structure_From_Template(*structure_config, Return_Neighboring_Tiles(grid_point), faction);
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

		current_num_entities++;

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

void Scene_Graph::Create_New_Scaffold(Coordinate grid_point, int structure_template_id, int faction)
{
	if (Check_Scaffold_Placement(grid_point, structure_template_id))
	{
		Structure_Template* structure_template = service_locator->get_Game_Library()->Fetch_Structure_Template(structure_template_id);

		// First we put an object in the scaffold array to represent our new scaffold and increment the number of scaffold in the world
		int array_index = 0;

		for (int i = 0; i < WORLD_MAX_NUM_SCAFFOLDS; i++)
		{
			// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
			if (scaffold_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
			{
				scaffold_array[i] = Object(i, { grid_point.x*TILE_SIZE, grid_point.y*TILE_SIZE, TILE_SIZE, TILE_SIZE }, service_locator);

				scaffold_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

				array_index = i;
				break;
			}
		}

		current_num_scaffolds++;

		tile_map[{grid_point.x, grid_point.y}].Update_Tile_Structure(&scaffold_array[array_index], TILE_LAYER_SCAFFOLD);

		// Now we initialize the object we just created in the scaffold array with an object_config
		scaffold_array[array_index].Init_Scaffold_From_Template(structure_template_id, faction);
	}
}

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

		current_num_projectiles++;

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
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x - 1, start.y, end.x - 1, end.y }, true, { 255,255,255,255 }, PRIMITIVE_TYPE_LINE);
	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, { start.x - 2, start.y, end.x - 2, end.y }, true, { laser_color.r,laser_color.g,laser_color.b,laser_color.a }, PRIMITIVE_TYPE_LINE);
}

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

		current_num_containers++;

		// Now we initialize the object we just created in the structure array with an object_config
		container_array[array_index].Init_Container_From_Inventory(inventory_array, num_items);

		if (pickup_flag == 1)
		{
			Job_Create_Pickup_Container(&container_array[array_index], 1);
		}
	}
}

void Scene_Graph::Draw()
{
	// Background will only re-draw from individual textures if there is movement on the screen, otherwise is a prebaked texture
	Draw_Background();

	Process_Main_Function(OBJECT_FUNCTION_DRAW);
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

void Scene_Graph::Delete_Structure_Update_Tile_Map_Send_Message(Coordinate grid_point, int tile_layer)
{
	// Then we tell the tile map to de-reference the object at that layer at the grid point specified
	// the tile map also sets the object that was previously there to unassigned so it can be overwritten
	tile_map[grid_point].Update_Tile_Structure(NULL, tile_layer);

	// Finally we send a SG tile update message is sent to the main bus outlining what happened
	int update_message[8] = { MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION , OBJECT_TYPE_ANY, FOCUS_ALL,grid_point.x,grid_point.y,tile_layer,0,0 };
	service_locator->get_MB_Pointer()->Add_Custom_Message(8, update_message);

	current_num_structures--;
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
		Delete_Structure_Update_Tile_Map_Send_Message(structure_array[array_num].get_coordinate(), service_locator->get_Game_Library()->Fetch_Structure_Template(structure_array[array_num].Return_Stats_Component()->Return_Template_ID())->tile_layer);
		break;
	case OBJECT_TYPE_ENTITY:
		entity_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		current_num_entities--;
		break;
	case OBJECT_TYPE_PROJECTILE:
		projectile_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		current_num_projectiles--;
		break;
	case OBJECT_TYPE_CONTAINER:
		container_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		current_num_containers--;
		break;
	case OBJECT_TYPE_SCAFFOLD:
		scaffold_array[array_num].Set_Assigned_Flag(OBJECT_UNASSIGNED);
		current_num_scaffolds--;
		break;
	}
}

void Scene_Graph::Delete_Scaffold(Coordinate grid_point)
{
	tile_map[grid_point].Update_Tile_Structure(NULL, TILE_LAYER_SCAFFOLD);
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

Goal_Set Scene_Graph::Goalset_Create_Move_Item_From_Object_To_Object(Item item, int amount_of_item, Object* start_object, Object* finish_object)
{
	Goal_Set new_goalset;
	int start_object_type = start_object->Return_Stats_Component()->Return_Object_Type();
	int finish_object_type = finish_object->Return_Stats_Component()->Return_Object_Type();

	int check_if_start_object_has_item[6] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_IF_OBJECT_HAS_ITEM, start_object_type, start_object->Get_Array_Index(), item.item_template_id, amount_of_item };
	int move_to_start_object[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, start_object_type, start_object->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, start_object_type, start_object->Get_Array_Index(),2 };
	int take_items_from_start_object[6] = { ACTION_TRANSFER, A_AT_TAKE_ITEM_FROM_OBJECT, start_object_type, start_object->Get_Array_Index(),item.item_template_id,amount_of_item };
	int set_carried_item[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_CARRIED_ITEM, item.item_template_id };

	int check_for_another_goalset[2] = { ACTION_ASSESS_INTERNAL, A_AI_CHECK_JOB_FOR_MORE_GOALSETS };
	int move_to_requestee[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, finish_object_type, finish_object->Get_Array_Index() };
	int check_distance_2[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, finish_object_type, finish_object->Get_Array_Index(),2 };
	int give_items_to_requestee[6] = { ACTION_TRANSFER, A_AT_GIVE_ITEM_TO_OBJECT, finish_object_type, finish_object->Get_Array_Index(), item.item_template_id, amount_of_item };
	int remove_carried_item[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_CARRIED_ITEM, 0 };

	int finish_goal_set[2] = { ACTION_CLOSE_OUT_GOAL_SET, new_goalset.goal_set_uniq_id };

	// Add the item check to make sure the entity doesn't waste time on the job 
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check for Item", check_if_start_object_has_item, 6);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Move to storage", move_to_start_object, 4);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance, 5);

	// Add th item check again to make sure they don't waste time again
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check for Item", check_if_start_object_has_item, 6);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Take Items", take_items_from_start_object, 6);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Set Carried Item", set_carried_item, 3);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check job", check_for_another_goalset, 2);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Move to requestee", move_to_requestee, 4);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance_2, 5);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Give items to requestee", give_items_to_requestee, 6);

	new_goalset.Add_Raw_Job_Goal_to_Goalset("Set No Carried Item", remove_carried_item, 3);
	new_goalset.Add_Raw_Job_Goal_to_Goalset("Finish goal set", finish_goal_set, 2);

	return new_goalset;
}

void Scene_Graph::Iterate_And_Function(Object object_array[], int current_num_members, int max_num_members, int object_function_type)
{
	int found_objects = 0;
	int array_index = 0;

	while (found_objects < current_num_members && array_index < max_num_members)
	{
		if (object_array[array_index].Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			//if (object_array[array_index].Return_Stats_Component()->Return_Object_Type() == OBJECT_TYPE_PROJECTILE) cout << "starting update of: " << array_index << endl;
			switch (object_function_type)
			{
			case OBJECT_FUNCTION_DRAW:
				object_array[array_index].Draw();
				break;
			case OBJECT_FUNCTION_UPDATE:

				object_array[array_index].Update();

				break;
			case OBJECT_FUNCTION_CHECK_FOR_MESSAGES:
				object_array[array_index].Collect_Bus_Messages();
				break;
			}

			found_objects++;
		}
		array_index++;
	}
}

Job* Scene_Graph::Job_Create_Pickup_Container(Object* container, int public_private, Object* assigned_object)
{
	Object* nearest_storage = Return_Nearest_Structure_By_Type(container, "STRUCTURE_TYPE_STORAGE");

	if (nearest_storage != NULL)
	{
		Goal_Set new_goalset;

		int move_to_container[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_CONTAINER, container->Get_Array_Index() };
		int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_CONTAINER, container->Get_Array_Index(),1 };
		int transfer_from_container[4] = { ACTION_TRANSFER, A_AT_TAKE_ALL_INVENTORY_FROM_OBJECT, OBJECT_TYPE_CONTAINER, container->Get_Array_Index() };
		int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET,new_goalset.goal_set_uniq_id };

		new_goalset.Add_Raw_Job_Goal_to_Goalset("Go to container", move_to_container, 4);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Check Distance", check_distance, 5);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Transfer Inventory From Container", transfer_from_container, 4);
		new_goalset.Add_Raw_Job_Goal_to_Goalset("Close Job", close_job, 2);

		vector<Goal_Set> goalset_vector;
		goalset_vector.push_back(new_goalset);

		return Add_Raw_Job_To_Job_Array(goalset_vector, 4, container, public_private);
	}
}

Job* Scene_Graph::Job_Create_Mine_Asteroid(Object* asteroid, int public_private, Object* assigned_object)
{
	Goal_Set new_goal_set;
	new_goal_set.max_num_assigned_objects = 2;

	int go_to_structure[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_STRUCTURE, asteroid->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_STRUCTURE, asteroid->Get_Array_Index(),1 };
	int stop_moving[2] = { ACTION_EDIT_INTERNAL , A_EI_STOP_MOVING };
	int fire_laser[3] = { ACTION_EDIT_EXTERNAL, A_EE_FIRE_MINING_LASER_AT_STRUCTURE, asteroid->Get_Array_Index() };
	int check_asteroid_health[8] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_OBJECT_STAT, LOWER_THAN_OR_EQUAL_TO, OBJECT_TYPE_STRUCTURE,asteroid->Get_Array_Index(), STAT_OBJECT_HEALTH, 0, 4 };
	int set_wait_timer[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, };
	int wait[2] = { ACTION_EDIT_INTERNAL, A_EI_WAIT };
	int go_back_four[2] = { ACTION_GOAL_TRAVERSAL, -4 };
	int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET, new_goal_set.goal_set_uniq_id };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go to asteroid", go_to_structure, 4);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check distance from asteroid", check_distance, 5);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Stop moving", stop_moving, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Fire mining laser", fire_laser, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check the asteroids health", check_asteroid_health, 8);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Set wait timer", set_wait_timer, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Wait", wait, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go back four steps", go_back_four, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Close job", close_job, 2);

	vector<Goal_Set> goalset_vector;
	goalset_vector.push_back(new_goal_set);

	return Add_Raw_Job_To_Job_Array(goalset_vector, 3, asteroid, public_private);
}

Job* Scene_Graph::Job_Create_Entity_Go_Change_Object_Stat(Object* requestee_object, int public_private, Object* assigned_object, int object_type, int stat, int comparator_function, int increment, int final_value)
{
	Goal_Set new_goal_set;
	new_goal_set.max_num_assigned_objects = 4;

	int go_to_scaffold[4] = { ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, object_type, requestee_object->Get_Array_Index() };
	int check_distance[5] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, object_type, requestee_object->Get_Array_Index(),1 };
	int stop_moving[2] = { ACTION_EDIT_INTERNAL , A_EI_STOP_MOVING };
	int increment_scaffold_built[6] = { ACTION_EDIT_EXTERNAL, A_EE_ADJUST_OBJECT_STAT, object_type, requestee_object->Get_Array_Index(), stat, increment };
	int check_scaffold_built_level[8] = { ACTION_ASSESS_EXTERNAL, A_AE_CHECK_OBJECT_STAT, comparator_function, object_type,requestee_object->Get_Array_Index(), stat,final_value, 4 };
	int set_wait_timer[3] = { ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, };
	int wait[2] = { ACTION_EDIT_INTERNAL, A_EI_WAIT };
	int go_back_four[2] = { ACTION_GOAL_TRAVERSAL, -4 };
	int close_job[2] = { ACTION_CLOSE_OUT_GOAL_SET,new_goal_set.goal_set_uniq_id };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go to Scaffold", go_to_scaffold, 4);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check distance from scaffold", check_distance, 5);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Stop moving", stop_moving, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Increment Scaffold Built", increment_scaffold_built, 6);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check the scaffold built level", check_scaffold_built_level, 8);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Set wait timer", set_wait_timer, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Wait", wait, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Go back four steps", go_back_four, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Close job", close_job, 2);

	vector<Goal_Set> new_goal_sets;
	new_goal_sets.push_back(new_goal_set);

	return Add_Raw_Job_To_Job_Array(new_goal_sets, 5, requestee_object, public_private);

}

bool Scene_Graph::Job_Create_Find_Blueprint_Items_And_Transport_To_Requestee(Object* requestee, Blueprint* blueprint, bool create_job, int is_public, Object* assigned_object)
{
	vector<Goal_Set> Goalsets_Item_Transport;

	for (int i = 0; i < blueprint->Num_Items_In_Blueprint; i++)
	{
		if (Create_Goalsets_To_Obtain_Item_Quantity_From_Multiple_Storage_Locations(Goalsets_Item_Transport, blueprint->blueprint_items[i], requestee) == false) return false;
	}

	if (create_job && Goalsets_Item_Transport.size() > 0)
	{
		Add_Raw_Job_To_Job_Array(Goalsets_Item_Transport, 6, requestee, is_public);
	}

	return true;
}

Job* Scene_Graph::Job_Create_Transport_Blueprint_Items_From_Object_To_Requestee(Object* start_object, Object* finish_object, Blueprint blueprint, int public_private, Object* assigned_object)
{
	vector<Goal_Set> new_goalset;
	for (int i = 0; i < blueprint.Num_Items_In_Blueprint; i++)
	{
		new_goalset.push_back(Goalset_Create_Move_Item_From_Object_To_Object(blueprint.blueprint_items[i].slot_item, blueprint.blueprint_items[i].item_quantity, start_object, finish_object));
	}

	if (assigned_object == NULL)
	{
		return Add_Raw_Job_To_Job_Array(new_goalset, 7, start_object, public_private);
	}
	else
	{
		return Add_Raw_Job_To_Job_Array(new_goalset, 7, assigned_object, public_private);
	}
}

Job* Scene_Graph::Job_Create_Local_Structure_Template_Job(Object* object, int job_id)
{
	switch (job_id)
	{
	case 1:
		return Job_Create_Local_Oxygenate(object);
		break;
	case 2:
		return Job_Create_Local_Open_Door(object);
		break;
	}
}

Job* Scene_Graph::Job_Create_Local_Oxygenate(Object* object)
{
	Goal_Set new_goal_set;

	int oxygenate[2] = { ACTION_ASSESS_EXTERNAL, A_AE_OXYGENATE };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Oxygenate Surroundings", oxygenate, 2);

	vector<Goal_Set> new_goal_sets;
	new_goal_sets.push_back(new_goal_set);

	return Add_Raw_Job_To_Job_Array(new_goal_sets, 1, object, 0);

}

Job* Scene_Graph::Job_Create_Local_Open_Door(Object* object)
{
	Goal_Set new_goal_set;

	int open_door[3] = { ACTION_EDIT_INTERNAL,	A_EI_OPEN_DOOR,	10 };
	int wait[2] = { ACTION_EDIT_INTERNAL,	A_EI_WAIT };
	int check_for_entities[4] = { ACTION_ASSESS_EXTERNAL,	A_AE_CHECK_FOR_ENTITIES_IN_RADIUS,	1, -1 };
	int close_door[2] = { ACTION_EDIT_INTERNAL, A_EI_CLOSE_DOOR };
	int pause[2] = { ACTION_EDIT_INTERNAL,A_EI_PAUSE_FOR_FURTHER_INSTRUCTIONS };

	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Open Door", open_door, 3);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Wait", wait, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Check for entities", check_for_entities, 4);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Close Door", close_door, 2);
	new_goal_set.Add_Raw_Job_Goal_to_Goalset("Pause", pause, 2);

	vector<Goal_Set> new_goal_sets;
	new_goal_sets.push_back(new_goal_set);

	return Add_Raw_Job_To_Job_Array(new_goal_sets, 2, object, 0);
}

void Scene_Graph::Process_Main_Function(int major_function_type)
{
	Iterate_And_Function(structure_array, current_num_structures, WORLD_MAX_NUM_STRUCTURES, major_function_type);
	Iterate_And_Function(scaffold_array, current_num_scaffolds, WORLD_MAX_NUM_SCAFFOLDS, major_function_type);
	Iterate_And_Function(entity_array, current_num_entities, WORLD_MAX_NUM_ENTITIES, major_function_type);
	Iterate_And_Function(projectile_array, current_num_projectiles, WORLD_MAX_NUM_PROJECTILES, major_function_type);
	Iterate_And_Function(container_array, current_num_containers, WORLD_MAX_NUM_CONTAINERS, major_function_type);
}

void Scene_Graph::Remove_Inventory_Slot_From_Manifest(Item_Slot* slot_pointer)
{
	for (int i = 0; i < inventory_manifest.size(); i++)
	{
		if (inventory_manifest[i].inventory_slot->uniq_id == slot_pointer->uniq_id)
		{
			Send_Inventory_Manifest_Update_Message(slot_pointer, inventory_manifest[i].associated_object, -1);
			inventory_manifest.erase(inventory_manifest.begin() + i);
			curent_num_item_slots_on_manifest--;
			break;
		}
	}
}

int Scene_Graph::Return_Num_Item_Slots_On_Manifest()
{
	return inventory_manifest.size();
}

int Scene_Graph::Return_Best_Item_Pickup_Location_From_Vector_Of_Locations(vector<Item_Location*> storage_locations_with_item)
{
	// We will improve this algorithm as it becomes necessary for entities not to do stupid shit

	return 0;
}

Job* Scene_Graph::Return_Job_With_Highest_Priority_Correlation(Object* requesting_object)
{
	if (current_num_jobs > 0)
	{
		for (int i = 0; i < WORLD_MAX_NUM_JOBS; i++)
		{

			if (current_job_array[i].Return_Is_Init() == true)
			{
				if ((current_job_array[i].Return_Job_Public_Private() == 1 || current_job_array[i].Return_Job_Requestor() == requesting_object) && current_job_array[i].Return_Num_Unassigned_Goalsets() > 0)
				{
					return &current_job_array[i];
				}

			}
		}
	}

	return NULL;
}

vector<Object*> Scene_Graph::Return_All_Entities_On_Tile(Coordinate tile)
{
	vector<Object*> entities_on_tile;
	for (int i = 0; i < current_num_entities; i++)
	{
		if (entity_array[i].get_coordinate().x == tile.x && entity_array[i].get_coordinate().y == tile.y)
		{
			entities_on_tile.push_back(&entity_array[i]);
		}
	}
	return entities_on_tile;
}

vector<Object*> Scene_Graph::Return_All_Entities_In_Radius(Coordinate base_coordinate, int radius)
{
	vector<Object*> all_entities_in_radius;

	for (int p = base_coordinate.y - radius; p < base_coordinate.y + radius + 1; p++)
	{
		for (int i = base_coordinate.x - radius; i < base_coordinate.x + radius + 1; i++)
		{
			vector<Object*> entities_on_tile = Return_All_Entities_On_Tile({ i, p });
			if (entities_on_tile.size() > 0)
			{
				for (int x = 0; x < entities_on_tile.size(); x++) all_entities_in_radius.push_back(entities_on_tile[x]);
			}
		}
	}

	return all_entities_in_radius;
}

vector<Item_Location*> Scene_Graph::Return_Vector_Of_Item_Locations(string iType, int item_id, int structure_type, int structure_id)
{
	vector<Item_Location*> item_locations;
	string item_type = iType;

	if (item_type.size() == 0) item_type = service_locator->get_Game_Library()->Fetch_Item_Template(item_id)->inventory_item_type_string;

	for (int i = 0; i < curent_num_item_slots_on_manifest; i++)
	{
		if (service_locator->get_Game_Library()->Fetch_Item_Template(inventory_manifest[i].inventory_slot->slot_item.item_template_id)->inventory_item_type_string == item_type)
		{
			bool include = true;

			if (item_id != 0 && inventory_manifest[i].inventory_slot->slot_item.item_template_id != item_id) include = false;
			if (structure_type != 0 && inventory_manifest[i].associated_object->Return_Stats_Component()->Get_Structure_Type() != structure_type) include = false;
			if (structure_id != 0 && inventory_manifest[i].associated_object->Return_Stats_Component()->Get_Structure_Name() != structure_id) include = false;

			if (include == true) item_locations.push_back(&inventory_manifest[i]);
		}

	}

	return item_locations;
}

int Scene_Graph::Return_Current_Num_Jobs_In_Array()
{
	return current_num_jobs;
}

int Scene_Graph::Return_Current_Num_Public_Jobs_In_Array()
{
	return current_num_public_jobs;
}

Object* Scene_Graph::Return_Nearest_Structure_By_Type(Object* local_object, string structure_type)
{
	Coordinate anchor_coord = local_object->get_coordinate();
	Object* temp_object = NULL;
	
	for (int i = 0; i < current_num_structures; i++)
	{
		AI_Stats_Component* object_stats = structure_array[i].Return_Stats_Component();

		if (object_stats->Get_Structure_Type() == service_locator->get_Game_Library()->Get_Structure_Type_Code_From_Structure_Type_String(structure_type))
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
		if (&structure_array[array_num] != NULL) return &structure_array[array_num];
		break;
	case OBJECT_TYPE_ENTITY:
		if (&entity_array[array_num] != NULL) return &entity_array[array_num];
		break;
	case OBJECT_TYPE_PROJECTILE:
		if (&projectile_array[array_num] != NULL) return &projectile_array[array_num];
		break;
	case OBJECT_TYPE_CONTAINER:
		if (&container_array[array_num] != NULL) return &container_array[array_num];
		break;
	case OBJECT_TYPE_SCAFFOLD:
		if (&scaffold_array[array_num] != NULL) return &scaffold_array[array_num];
		break;
	}

	cout << "trying to return object of type: " << object_type << " that doesn't exist in Return Object by Type and Array Num Function" << endl;
	return NULL;
}

Object* Scene_Graph::Return_Structure_By_Array_Num(int array_num)
{
	return &structure_array[array_num];
}

Object* Scene_Graph::Return_Entity_By_Array_Num(int array_num)
{
	return &entity_array[array_num];
}

Coordinate Scene_Graph::Return_Nearest_Accessible_Coordinate(Coordinate origin, Coordinate destination, int requesting_faction)
{
	Coordinate delta[] = { { 0,1 },{ 1,1 },{ -1,1 },{ -1,0 },{ 1,0 },{-1,-1 },{ 1,-1 },{ 0,-1 }};
	
	if (origin.x == destination.x && origin.y == destination.y)
	{
		for (int i = 0; i < 8; i++)
		{
			if (!Check_If_Tile_Is_Inaccessible({ origin.x + delta[i].x, origin.y + delta[i].y }, requesting_faction))
			{
				return { origin.x + delta[i].x, origin.y + delta[i].y };
			}
		}

		service_locator->get_UI_pointer()->Push_Message_To_Console("Production blocked, nowhere to put item");
	}
	else
	{
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


		while (Check_If_Tile_Is_Inaccessible(d, requesting_faction))
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

void Scene_Graph::Return_Tiles_Without_Leaks(Coordinate start_tile, vector<Coordinate> &tiles_to_oxygenate, map<Coordinate, bool> &checked_tiles, bool &is_leak)
{
	checked_tiles[start_tile] = true;

	if (!Check_If_Tile_Has_Leak(start_tile) && is_leak == false)
	{
		tiles_to_oxygenate.push_back(start_tile);
		for (int p = -1; p < 2; p++)
		{
			for (int i = -1; i < 2; i++)
			{
				if (!(i == 0 && p == 0))
				{
					Coordinate new_start_tile = { start_tile.x + i, start_tile.y + p };
					if (checked_tiles.count(new_start_tile) == 0 && !Check_If_Tile_Is_Wall_Or_Closed_Door(new_start_tile))
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

void Scene_Graph::Send_Inventory_Manifest_Update_Message(Item_Slot* slot_pointer, Object* associated_object, int inventory_update_type)
{
	int update_message[7] = { MESSAGE_TYPE_SG_INVENTORY_MANIFEST_UPDATE, OBJECT_TYPE_ANY, FOCUS_ALL, inventory_update_type,slot_pointer->uniq_id, associated_object->Return_Stats_Component()->Return_Object_Type(), associated_object->Get_Array_Index() };
	service_locator->get_MB_Pointer()->Add_Custom_Message(7, update_message);
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

void Scene_Graph::Update()
{
	Process_Main_Function(OBJECT_FUNCTION_UPDATE);
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
