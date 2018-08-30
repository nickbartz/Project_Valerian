#include<Scene_Graph.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<iostream>

Scene_Graph::Scene_Graph(Service_Locator* sLocator)
{
	service_locator = sLocator;
	Create_Background();
}

void Scene_Graph::Create_Background()
{
	background_star_1 = Object(0,{ 0,0,0,0 },service_locator);
	Object_Config background_star_1_object_config;
	background_star_1_object_config.spritesheet = SPRITESHEET_BACKGROUND;
	background_star_1_object_config.tile_clip = { 0,0,32,32 };
	background_star_1.Assign_Background_Renderer(service_locator, background_star_1_object_config);

	background_star_2 = Object(0,{ 0,0,0,0 }, service_locator);
	Object_Config background_star_2_object_config;
	background_star_2_object_config.spritesheet = SPRITESHEET_BACKGROUND;
	background_star_2_object_config.tile_clip = { 32,0,32,32 };
	background_star_2.Assign_Background_Renderer(service_locator, background_star_2_object_config);

	background_planetoid = Object(0,{ 0,0,0,0 }, service_locator);
	Object_Config background_planetoid_config;
	background_planetoid_config.spritesheet = SPRITESHEET_BACKGROUND;
	background_planetoid_config.tile_clip = { 0,32,32,32 };
	background_planetoid.Assign_Background_Renderer(service_locator, background_planetoid_config);

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

		if (background_objects[i].type == 0) background_star_1.Draw(camera, pos_rect);
		else if (background_objects[i].type == 1) background_star_2.Draw(camera, pos_rect);
		else if (background_objects[i].type == 3) background_planetoid.Draw(camera, pos_rect);
	}
}

void Scene_Graph::Create_New_Structure(Coordinate grid_point, Object_Config structure_config)
{
	// First we put an object in the structure array to represent our new structure and increment the number of structures in the world
	int array_index = 0;
	current_num_structures++;

	for (int i = 0; i < WORLD_MAX_NUM_STRUCTURES; i++)
	{
		// we place the object in the first array index with an "OBJECT_UNASSIGNED" type so we can overwrite objects no longer being used
		if (structure_array[i].Get_Assigned_Flag() == OBJECT_UNASSIGNED)
		{
			structure_array[i] = Object(i, { 0,0,0,0 }, service_locator);

			structure_array[i].Set_Assigned_Flag(OBJECT_ASSIGNED);

			array_index = i;
			break;
		}
	}
	

	// Now we edit the object we just created in the structure array
	Assign_AI_Components(&structure_array[array_index], structure_config);
	Assign_Physics_Components(&structure_array[array_index], structure_config);
	Assign_Render_Components(&structure_array[array_index], structure_config);

	// We update the tile map with a pointer to the new object we just created which sends a 
	Update_Tile_Map(grid_point, structure_config.tile_layer, &structure_array[array_index]);

	// Finally we send a SG tile update message is sent to the main bus outlining what happened
	service_locator->get_MB_Pointer()->Add_Message(Message_SG_Tile_Update(grid_point.x, grid_point.y, structure_config.tile_layer, structure_config.structure_id, structure_config.structure_type));
}

void Scene_Graph::Assign_AI_Components(Object* object, Object_Config object_config)
{
	object->Assign_Core_AI_Component(service_locator, object_config);
	
	if (object_config.object_type == OBJECT_TYPE_STRUCTURE)
	{
		object->Assign_Basic_Structure_AI_Component(service_locator, object_config);
	}
}

void Scene_Graph::Assign_Physics_Components(Object* object, Object_Config object_config)
{

}

void Scene_Graph::Assign_Render_Components(Object* object, Object_Config object_config) 
{
	object->Assign_Overlay_Renderer();

	Adjacent_Structure_Array neighbors = {};

	switch (object_config.render_component_type)
	{
	case RENDER_COMPONENT_BACKGROUND:
		object->Assign_Background_Renderer(service_locator, object_config);
		break;
	case RENDER_COMPONENT_SIMPLECLIP:
		object->Assign_Simple_Clip_Tile_Renderer(service_locator, object_config);
		break;
	case RENDER_COMPONENT_MULTICLIP:
		object->Assign_Multi_Clip_Tile_Renderer(service_locator, object_config, neighbors);
		break;
	}
}

void Scene_Graph::Update_Tile_Map(Coordinate grid_point, int tile_layer, Object* structure)
{
	// If there isn't a tile at those coordinates, make a tile
	if (tile_map.count(grid_point) == 0) tile_map[grid_point] = Tile(grid_point);

	// Update the pointers to the object at that layer at that gridpoint, if a pointer to an object already exists, the objects "type" will be set to "OBJECT_UNASSIGNED" so it can be overwritten
	// if the update process returns true, that means there was already an assigned structure in that slot in that tile, the current num structures is reduced by 1 to account for its removal

	if (tile_layer == TILE_LAYER_BASE) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_BASE) == true) current_num_structures--;
		
	if (tile_layer == TILE_LAYER_MID) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_BASE) == true) current_num_structures--;

	if (tile_layer == TILE_LAYER_ROOF) if (tile_map[grid_point].Update_Tile_Structure(structure, TILE_LAYER_BASE) == true) current_num_structures--;

}

void Scene_Graph::Draw()
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();
	
	Draw_Background();

	for (int i = 0; i < current_num_structures; i++) structure_array[i].Draw(camera, { 0,0,0,0 });

}

// Accessors 

int Scene_Graph::Return_Current_Structure_Count()
{
	return current_num_structures;
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