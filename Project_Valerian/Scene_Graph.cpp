#include<Scene_Graph.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<iostream>

Scene_Graph::Scene_Graph(Service_Locator* sLocator)
{
	service_locator = sLocator;

	background_star_1 = Object(sLocator, 0, 0, 1, 1);
	background_star_1.Assign_Simple_Clip_Renderer(SPRITESHEET_BACKGROUND, { 0,0,32,32 });

	background_star_2 = Object(sLocator, 0, 0, 1, 1);
	background_star_2.Assign_Simple_Clip_Renderer(SPRITESHEET_BACKGROUND, { 32,0,32,32 });

	background_planetoid = Object(sLocator, 0, 0, 1, 1);
	background_planetoid.Assign_Simple_Clip_Renderer(SPRITESHEET_BACKGROUND, { 0,32,32,32 });

	for (int i = 0; i < WORLD_MAX_NUM_BACKGROUND_OBJECTS; i++)
	{
		background_objects[i] = { rand() % SCREEN_WIDTH - SCREEN_WIDTH/2, rand() % SCREEN_HEIGHT - SCREEN_HEIGHT/2, rand() % 10 + 1,rand() % 2 };
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

		if (background_objects[i].type == 0) background_star_1.Draw_From_Instance(pos_rect);
		else if (background_objects[i].type == 1) background_star_2.Draw_From_Instance(pos_rect);
		else if (background_objects[i].type == 3) background_planetoid.Draw_From_Instance(pos_rect);
	}
}

void Scene_Graph::Draw()
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();
	
	Draw_Background();

	for (int i = 0; i < current_num_structures; i++)
	{
		structure_array[i].Draw(camera);
	}
}