#include<Draw_System.h>
#include<Service_Locator.h>
#include<Render_Component.h>
#include<iostream>
#include<SDL.h>
#include<Cursor.h>
#include<algorithm>

using namespace std;

Core_Render::Core_Render(Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Core_Render::Draw()
{

}

void Background_Renderer::Draw(SDL_Rect pos_rect)
{
	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, pos_rect, sprite_clip, angle, center, flip);
}

void Simple_Clip_Tile_Renderer::Draw(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w/TILE_SIZE) + SCREEN_WIDTH/2 + camera.x, (pos_rect.y*camera.w/TILE_SIZE) + SCREEN_HEIGHT/2 + camera.y, tile_coords.w*camera.w, tile_coords.h*camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, draw_rect, sprite_clip, angle, center,flip);
}

void Multisprite_Tile_Renderer::Initialize_Dedicated_Multisprite()
{
	// This creates a dedicated spritesheet for this multitileconfig with a blank SDL_Texture the size of one tile
	dedicated_multisprite_num = service_locator->get_Draw_System_Pointer()->Add_New_Spritesheet_To_Multisprite(spritesheet_num, service_locator->get_Game_Renderer());
	if (dedicated_multisprite_num >= 0 && dedicated_multisprite_num <= MAX_SPRITES_PER_MULTISPRITE) init = true;
}

void Multisprite_Tile_Renderer::Deinitialize_Dedicated_Multisprite()
{
	// This frees up the spritesheet currently being used and puts it back into free rotation for others
	service_locator->get_Draw_System_Pointer()->Remove_Multisprite(spritesheet_num,dedicated_multisprite_num);
	init = false;
}

bool Multisprite_Tile_Renderer::Is_Init()
{
	return init;
}

void Multisprite_Tile_Renderer::Adjust_Multisprite_To_Surroundings(Adjacent_Structure_Array neighbors)
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet_num, dedicated_multisprite_num, { 0,0,32,32 });
}

void Multisprite_Tile_Renderer::Check_Bus_For_Surrounding_Tile_Updates()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_SG_tile_map_update_messages; i++)
	{
		int message_tile_x = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_X();
		int message_tile_y = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_Y();

		int delta_x = message_tile_x - tile_coords.x;
		int delta_y = message_tile_y - tile_coords.y;

		if (abs(delta_x <= 1) && abs(delta_y <= 1) && !(delta_x == 0 && delta_y == 0))
		{
			int message_tile_layer = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Tile_Layer();
			int message_tile_type = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Structure_Type();
			adjacent_tile_type_array.adjacent_structure_array[message_tile_layer][delta_x + 1][delta_y + 1] = message_tile_type;
		}
	}
}

void Multisprite_Tile_Renderer::Draw(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, tile_coords.w*camera.w, tile_coords.h*camera.w };
	
	service_locator->get_Draw_System_Pointer()->Add_Multisprite_Render_Job_To_Render_Cycle(spritesheet_num, dedicated_multisprite_num, draw_rect);
}

