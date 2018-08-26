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

void Simple_Clip_Tile_Renderer::Draw()
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect pos_rect = { (tile_specs.x*camera.w) + SCREEN_WIDTH + camera.x, (tile_specs.y*camera.w) + SCREEN_HEIGHT + camera.y, tile_specs.w*camera.w, tile_specs.h*camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, pos_rect, sprite_clip, angle, center,flip);
}

void Multi_Clip_Tile_Renderer::Check_Bus_For_Surrounding_Tile_Updates()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_SG_tile_map_update_messages; i++)
	{
		int message_tile_x = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_X();
		int message_tile_y = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_Y();

		int delta_x = message_tile_x - grid_x;
		int delta_y = message_tile_y - grid_y;

		if (abs(delta_x <= 1) && abs(delta_y <= 1) && !(delta_x == 0 && delta_y == 0))
		{
			int message_tile_layer = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Tile_Layer();
			int message_tile_type = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Structure_Type();
			Surrounding_Tile_Array[message_tile_layer][delta_x + 1][delta_y + 1] = message_tile_type;
		}
	}
}

void Multi_Clip_Tile_Renderer::Draw()
{

}

