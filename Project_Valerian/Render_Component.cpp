#include<Draw_System.h>
#include<Service_Locator.h>
#include<Render_Component.h>
#include<iostream>
#include<SDL.h>
#include<Cursor.h>
#include<algorithm>
#include<Game_Library.h>

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

void Multisprite_Tile_Renderer::Update_Neighbor_Array(Adjacent_Structure_Array new_neighbors)
{
	neighbors = new_neighbors;
}

void Multisprite_Tile_Renderer::Adjust_Multisprite_To_Surroundings()
{
	switch (multi_clip_type)
	{
	case MULTI_CLIP_FLOOR:
		Stamp({ 0,0,32,32 }, { 0,0,32,32 });
		break;
	case MULTI_CLIP_WALL:
		Build_Wall_Multisprite();
		break;
	}
}

void Multisprite_Tile_Renderer::Check_Bus_For_Surrounding_Tile_Updates()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array.size(); i++)
	{
		int message_tile_x = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_X();
		int message_tile_y = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Grid_Y();

		int delta_x = message_tile_x - tile_coords.x;
		int delta_y = message_tile_y - tile_coords.y;

		if (abs(delta_x) <= 1 && abs(delta_y) <= 1 && !(delta_x == 0 && delta_y == 0))
		{
			int message_tile_layer = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Tile_Layer();
			int message_tile_type = service_locator->get_MB_Pointer()->SG_Tile_Update_MSG_Array[i].Get_Structure_Type();
	
			neighbors.asa[message_tile_layer-1][delta_x + 1][delta_y + 1] = message_tile_type;

			Adjust_Multisprite_To_Surroundings();
		}
	}
}

void Multisprite_Tile_Renderer::Draw(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, tile_coords.w*camera.w, tile_coords.h*camera.w };
	
	service_locator->get_Draw_System_Pointer()->Add_Multisprite_Render_Job_To_Render_Cycle(spritesheet_num, dedicated_multisprite_num, draw_rect);
}

void Multisprite_Tile_Renderer::Stamp(SDL_Rect spritesheet_clip, SDL_Rect pos_rect, int tile_offset_x, int tile_offset_y)
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet_num, dedicated_multisprite_num, spritesheet_clip, pos_rect);
}

void Multisprite_Tile_Renderer::Clear_Sprite()
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet_num, dedicated_multisprite_num, { 0,0,0,0 }, { 0,0,0,0 }, true);
}

void Multisprite_Tile_Renderer::Build_Wall_Multisprite()
{
	Clear_Sprite();
	
	Game_Library* gl = service_locator->get_Game_Library();

	int num_surrounding_vacuum = 0;
	int num_surrounding_walls = 0;
	int num_diagonal_walls = 0;
	int num_diagonal_vacuum = 0;

	// Check to see if there's an adjacent tile that is vacuum, if so, send this to the function that handles exterior tiles
	for (int i = 0; i < 3; i++) for (int p = 0; p < 3; p++) if ((abs(i - 1) != abs(p - 1))) if (gl->is_null(neighbors.asa[0][p][i]) && gl->is_null(neighbors.asa[1][p][i])) num_surrounding_vacuum++;
	for (int i = 0; i < 3; i++) for (int p = 0; p < 3; p++) if ((abs(i - 1) == abs(p - 1)) && !(i == 1 && p == 1))
	{
		if (gl->is_null(neighbors.asa[0][p][i]) && gl->is_null(neighbors.asa[1][p][i])) num_diagonal_vacuum++;
	}
	for (int i = 0; i < 3; i++) for (int p = 0; p < 3; p++) if ((abs(i - 1) != abs(p - 1))) if (gl->is_wall(neighbors.asa[1][p][i])) num_surrounding_walls++;
	for (int i = 0; i < 3; i++) for (int p = 0; p < 3; p++) if ((abs(i - 1) == abs(p - 1)) && !(i == 0 && p == 0)) if (gl->is_wall(neighbors.asa[1][p][i])) num_diagonal_walls++;

	if (num_diagonal_vacuum == 1 && num_surrounding_vacuum == 0) Build_Wall_Multisprite_Handle_Complex_Exterior_Walls(num_surrounding_walls, num_diagonal_vacuum);
	else if (num_surrounding_vacuum > 0) Build_Wall_Multisprite_Handle_Exterior_Walls(num_surrounding_vacuum, num_surrounding_walls);
	else Build_Wall_Multisprite_Handle_Simple_Interior_Walls(num_surrounding_walls);
}

void Multisprite_Tile_Renderer::Build_Wall_Multisprite_Handle_Exterior_Walls(int num_space_walls, int num_surrounding_walls)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TW = TILE_SIZE;
	int cto_x = 0;
	int cto_y = 0;

	// Only one of the tiles around is vacuum
	if (num_space_walls == 1)
	{
		if (num_surrounding_walls < 2)
		{

		}
		else if (num_surrounding_walls == 2)
		{
			// One space wall is left
			if (gl->is_null(neighbors.asa[1][0][1]) && gl->is_null(neighbors.asa[0][0][1])) Stamp({ 0 * TW,2 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// One space wall is right
			else if (gl->is_null(neighbors.asa[1][2][1]) && gl->is_null(neighbors.asa[0][2][1])) Stamp({ 7 * TW,2 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// One space wall is above
			else if (gl->is_null(neighbors.asa[1][1][0]) && gl->is_null(neighbors.asa[0][1][0]))  Stamp({ 1 * TW,0 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// One space wall is below
			else if (gl->is_null(neighbors.asa[1][1][2]) && gl->is_null(neighbors.asa[0][1][2]))  Stamp({ 1 * TW,7 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
		else if (num_surrounding_walls == 3)
		{
			// Space wall is left and there's a wall to the right
			if (gl->is_null(neighbors.asa[1][0][1])) Stamp({ 0 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// Space wall is right and there's a wall to the left
			else if (gl->is_null(neighbors.asa[1][2][1])) Stamp({ 7 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// Space wall is above and there's a wall below
			else if (gl->is_null(neighbors.asa[1][1][0])) Stamp({ 2 * TW,0 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

			// Space wall is below and there's a wall above
			else if (gl->is_null(neighbors.asa[1][1][2])) Stamp({ 5 * TW,7 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
		

	}
	// Two of the tiles around is vacuum
	else if (num_space_walls == 2)
	{
		// The space is above and to the right
		if (gl->is_null(neighbors.asa[1][1][0]) && gl->is_null(neighbors.asa[0][1][0]) && gl->is_null(neighbors.asa[1][2][1]) && gl->is_null(neighbors.asa[0][2][1]))
		{
			Stamp({ 7 * TW,0 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
		// The space is above and to the left
		else if (gl->is_null(neighbors.asa[1][1][0]) && gl->is_null(neighbors.asa[0][1][0]) && gl->is_null(neighbors.asa[1][0][1]) && gl->is_null(neighbors.asa[0][0][1]))
		{
			Stamp({ 0 * TW,0 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
		// Space is below and to the left
		else if (gl->is_null(neighbors.asa[1][1][2]) && gl->is_null(neighbors.asa[0][1][2]) && gl->is_null(neighbors.asa[1][0][1]) && gl->is_null(neighbors.asa[0][0][1]))
		{
			Stamp({ 0 * TW,7 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
		// Space is below and to the right
		else if (gl->is_null(neighbors.asa[1][1][2]) && gl->is_null(neighbors.asa[0][1][2]) && gl->is_null(neighbors.asa[1][2][1]) && gl->is_null(neighbors.asa[0][2][1]))
		{
			Stamp({ 7 * TW,7 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
		}
	}
	// Three of the tiles around is vacuum
	else if (num_space_walls == 3)
	{
		// Need to add these for blocks that are stranded in space
		Stamp({ 1 * TW,4 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}
	// All four of the tiles around is vacuum
	else if (num_space_walls == 4)
	{
		// need to add these for blocks that are stranded in space
		Stamp({ 1 * TW,4 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}


}

void Multisprite_Tile_Renderer::Build_Wall_Multisprite_Handle_Simple_Interior_Walls(int num_surrounding_walls)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TQ = TILE_SIZE / 2; // TQ is a unit representing a quarter of a tile
	int TW = TILE_SIZE;
	int cto_x = 0;
	int cto_y = 0;

	// if everything around the tile is a wall 
	if (num_surrounding_walls == 4)
	{
		Stamp({ 2 * TW,2 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}
	
	// if there are no walls around the tile 
	else if (num_surrounding_walls == 0) Stamp({ 1 * TW,3 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

	// if there is one wall around the tile
	else if (num_surrounding_walls == 1)
	{
		// If the wall is to the left
		if (gl->is_wall(neighbors.asa[1][0][1])) Stamp({ 3 * TW,2 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// If the wall is to the right
		if (gl->is_wall(neighbors.asa[1][2][1])) Stamp({ 1 * TW,2 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// If the wall is above
		if (gl->is_wall(neighbors.asa[1][1][0])) Stamp({ 2 * TW,4 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// If the wall is below
		if (gl->is_wall(neighbors.asa[1][1][2])) Stamp({ 3 * TW,3 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}

	// if there are two walls around the tile
	else if (num_surrounding_walls == 2)
	{
		// There is wall above and below
		if (gl->is_wall(neighbors.asa[1][1][0]) && gl->is_wall(neighbors.asa[1][1][2])) Stamp({ 2 * TW,3 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// There is wall the the left and right
		if (gl->is_wall(neighbors.asa[1][0][1]) && gl->is_wall(neighbors.asa[1][2][1])) Stamp({ 1 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// There is wall the the left and above
		if (gl->is_wall(neighbors.asa[1][0][1]) && gl->is_wall(neighbors.asa[1][1][0])) Stamp({ 3 * TW,6 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// There is wall the the right and above
		if (gl->is_wall(neighbors.asa[1][2][1]) && gl->is_wall(neighbors.asa[1][1][0])) Stamp({ 1 * TW,6 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// There is wall the the left and below
		if (gl->is_wall(neighbors.asa[1][0][1]) && gl->is_wall(neighbors.asa[1][1][2])) Stamp({ 3 * TW,5 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// There is wall the the right and below
		if (gl->is_wall(neighbors.asa[1][2][1]) && gl->is_wall(neighbors.asa[1][1][2])) Stamp({ 1 * TW,5 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}

	else if (num_surrounding_walls == 3)
	{
		// the 3 surrounding walls are to the left
		if (!gl->is_wall(neighbors.asa[1][2][1])) Stamp({ 5* TW,4 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// the 3 surrounding walls are to the right
		if (!gl->is_wall(neighbors.asa[1][0][1])) Stamp({ 5 * TW,3 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// the 3 surrounding walls are above
		if (!gl->is_wall(neighbors.asa[1][1][2])) Stamp({ 6 * TW,3 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

		// the 3 surrounding walls are below`
		if (!gl->is_wall(neighbors.asa[1][1][0])) Stamp({ 5 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);
	}

}

void Multisprite_Tile_Renderer::Build_Wall_Multisprite_Handle_Complex_Exterior_Walls(int num_surrounding_walls, int num_diagonal_vacuum)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TQ = TILE_SIZE / 2; // TQ is a unit representing a quarter of a tile
	int TW = TILE_SIZE;
	int cto_x = 0;
	int cto_y = 0;

	// If the diagonal vacuum is in the top left
	if (gl->is_null(neighbors.asa[0][0][0]) && gl->is_null(neighbors.asa[1][0][0])) Stamp({ 6 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

	// If the diagonal vacuum is in the top right
	else if (gl->is_null(neighbors.asa[0][2][0]) && gl->is_null(neighbors.asa[1][2][0])) Stamp({ 4 * TW,1 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

	// If the diagonal vacuum is in the bottom left
	else if (gl->is_null(neighbors.asa[0][0][2]) && gl->is_null(neighbors.asa[1][0][2])) Stamp({ 6 * TW,6 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);

	// If the diagonal vacuum is in the bottom right
	else if (gl->is_null(neighbors.asa[0][2][2]) && gl->is_null(neighbors.asa[1][2][2])) Stamp({ 4 * TW,6 * TW,TW,TW }, { 0,0,TW,TW }, cto_x, cto_y);



}

void Multisprite_Tile_Renderer::Build_Floor_Multisprite()
{

}