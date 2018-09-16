#include<Draw_System.h>
#include<Service_Locator.h>
#include<Render_Component.h>
#include<iostream>
#include<SDL.h>
#include<Cursor.h>
#include<algorithm>
#include<Game_Library.h>
#include<AI_Stats_Component.h>>

using namespace std;

// Init for Structures
Render_Component::Render_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator,  Structure_Template object_config, Adjacent_Structure_Array neighbor_array)
{
	render_component = object_config.render_component_type;
	multiclip_type = object_config.multiclip_type;
	spritesheet = object_config.spritesheet;

	service_locator = sLocator;
	object_locator = oLocator;

	sprite_clip = object_config.tile_clip;
	sprite_coords = object_config.tile_specs;

	max_structure_animation_frames = object_config.num_animation_frame;

	if (render_component == RENDER_COMPONENT_MULTICLIP)
	{
		neighbors = neighbor_array;
		Initialize_Dedicated_Multisprite();
		Adjust_Multisprite_To_Surroundings();
	}
}

// Init for Entities
Render_Component::Render_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template entity_config)
{
	service_locator = sLocator;
	object_locator = oLocator;
	
	render_component = entity_config.render_component;
	num_entity_animations = entity_config.num_entity_animations;
	num_entity_components = entity_config.num_entity_components;

	for (int i = 0; i < entity_config.num_entity_animations; i++)
	{
		for (int p = 0; p < entity_config.num_entity_components; p++)
		{
			entity_animations[i][p] = entity_config.entity_animations[i][p];
		}
	}
}

// Main draw function
void Render_Component::Draw(SDL_Rect pos_rect)
{	
	switch (render_component)
	{
	case RENDER_COMPONENT_NONE:
		break;
	case RENDER_COMPONENT_BACKGROUND:
		Draw_With_Background_Renderer(pos_rect);
		break;
	case RENDER_COMPONENT_SIMPLECLIP:
		Draw_With_Simple_Clip(pos_rect);
		break;
	case RENDER_COMPONENT_MULTICLIP:
		Draw_With_Multi_Clip(pos_rect);
		break;
	case RENDER_COMPONENT_ANIMATED_CLIP:
		Draw_With_Animated_Simple_Clip(pos_rect);
		break;
	case RENDER_COMPONENT_ENTITY_SIMPLE:
		break;
	case RENDER_COMPONENT_ENTITY_COMPLEX:
		Draw_With_Complex_Entity_Animation(pos_rect);
		break;
	}

	Draw_Overlays(pos_rect);
}

// Component draw functions
void Render_Component::Draw_With_Background_Renderer(SDL_Rect pos_rect)
{
	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, pos_rect, sprite_clip, 0.0, NULL, SDL_FLIP_NONE);
}

void Render_Component::Draw_With_Simple_Clip(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, sprite_clip, 0.0, NULL, SDL_FLIP_NONE);

	// If the sprite is several stories high, the 2nd story needs to be printed seperately and later so that i will appear to float above any people walking around so they appear to go behind it
	if (sprite_coords.h == 2 && spritesheet == SPRITESHEET_MID_1)
	{
		// As a first step, change the clip to the 2nd story of the sprite on the sprite sheet 
		SDL_Rect new_clip = { sprite_clip.x, sprite_clip.y - SPRITE_SIZE, sprite_clip.w, sprite_clip.h };

		// Now re-do the draw rect and send a new instruction to the draw system to draw that 2nd story 
		draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, ((pos_rect.y - TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(SPRITESHEET_MID_2, draw_rect, new_clip, 0.0, NULL, SDL_FLIP_NONE);
	}
}

void Render_Component::Draw_With_Multi_Clip(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, sprite_coords.w*camera.w, sprite_coords.h*camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Multisprite_Render_Job_To_Render_Cycle(spritesheet, dedicated_multisprite_num, draw_rect);
}

void Render_Component::Draw_With_Animated_Simple_Clip(SDL_Rect pos_rect)
{	

	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect anim_clip = { sprite_clip.x + structure_animation_frame * SPRITE_SIZE, sprite_clip.y, sprite_clip.w, sprite_clip.h };

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, anim_clip, 0.0, NULL, SDL_FLIP_NONE);

	// If the sprite is several stories high, the 2nd story needs to be printed seperately and later so that i will appear to float above any people walking around so they appear to go behind it
	if (sprite_coords.h == 2 && spritesheet == SPRITESHEET_MID_1)
	{
		// As a first step, change the clip to the 2nd story of the sprite on the sprite sheet 
		SDL_Rect new_clip = { sprite_clip.x + structure_animation_frame, sprite_clip.y - SPRITE_SIZE, sprite_clip.w, sprite_clip.h };

		// Now re-do the draw rect and send a new instruction to the draw system to draw that 2nd story 
		draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, ((pos_rect.y - TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(SPRITESHEET_MID_2, draw_rect, new_clip, 0.0, NULL, SDL_FLIP_NONE);
	}
}

void Render_Component::Draw_With_Complex_Entity_Animation(SDL_Rect pos_rect)
{
	for (int i = 0; i < num_entity_components; i++)
	{
		Draw_Entity_Animation_Component(pos_rect, entity_animations[current_entity_anim][i].component_clip, entity_animations[current_entity_anim][i].component_current_frame, SPRITESHEET_ENTITY);
	}
}

// Draw function sub-functions

void Render_Component::Draw_Entity_Animation_Component(SDL_Rect pos_rect, SDL_Rect sprite_clip, int animation_frame, int spritesheet)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect anim_clip = { sprite_clip.x + animation_frame * SPRITE_SIZE, sprite_clip.y, sprite_clip.w, sprite_clip.h };

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, anim_clip, 0.0, NULL, SDL_FLIP_NONE);
}

void Render_Component::Draw_Overlays(SDL_Rect pos_rect)
{
	if (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type() == OBJECT_TYPE_STRUCTURE)
	{
		int oxygen_level = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_OXYGEN_LEVEL);
		if (oxygen_level > 0) Handle_Oxygenation_Overlay(pos_rect, oxygen_level);
	}
}

void Render_Component::Handle_Oxygenation_Overlay(SDL_Rect pos_rect, int oxygenation_level)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	SDL_Color overlay_color = { 0,0,255,oxygenation_level * 5 };

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, draw_rect, true, overlay_color);
}

// Functions that support multisprites

void Render_Component::Initialize_Dedicated_Multisprite()
{
	// This creates a dedicated spritesheet for this multitileconfig with a blank SDL_Texture the size of one tile
	dedicated_multisprite_num = service_locator->get_Draw_System_Pointer()->Add_New_Spritesheet_To_Multisprite(spritesheet, service_locator->get_Game_Renderer());
	if (dedicated_multisprite_num >= 0 && dedicated_multisprite_num <= MAX_SPRITES_PER_MULTISPRITE) init = true;
}

void Render_Component::Deinitialize_Dedicated_Multisprite()
{
	// This frees up the spritesheet currently being used and puts it back into free rotation for others
	service_locator->get_Draw_System_Pointer()->Remove_Multisprite(spritesheet,dedicated_multisprite_num);
	init = false;
}

bool Render_Component::Is_Init()
{
	return init;
}

void Render_Component::Update_Neighbor_Array(Adjacent_Structure_Array new_neighbors)
{
	neighbors = new_neighbors;
}

void Render_Component::Adjust_Multisprite_To_Surroundings()
{
	switch (multiclip_type)
	{
	case MULTI_CLIP_FLOOR:
		Stamp({ 0,0,32,32 }, { 0,0,32,32 });
		break;
	case MULTI_CLIP_WALL:
		Build_Wall_Multisprite();
		break;
	}
}

void Render_Component::Check_For_Messages(int grid_x, int grid_y)
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		if (service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(0) == MESSAGE_TYPE_SG_TILE_UPDATE)
		{
			Check_Tile_Update_Message(&service_locator->get_MB_Pointer()->Custom_Message_Array[i]);
		}
	}
}

void Render_Component::Check_Tile_Update_Message(Custom_Message* tile_update)
{
	int grid_x = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_GRID_X);
	int grid_y = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_GRID_Y);
	
	bool sprite_update = false;

	int message_tile_x = tile_update->Read_Message(3);
	int message_tile_y = tile_update->Read_Message(4);

	int delta_x = message_tile_x - grid_x;
	int delta_y = message_tile_y - grid_y;

	if (abs(delta_x) <= 1 && abs(delta_y) <= 1 && !(delta_x == 0 && delta_y == 0))
	{
		int message_tile_layer = tile_update->Read_Message(5);
		int message_tile_type = tile_update->Read_Message(6);

		neighbors.asa[message_tile_layer - 1][delta_x + 1][delta_y + 1] = message_tile_type;
		sprite_update = true;
	}

	if (sprite_update == true) Adjust_Multisprite_To_Surroundings();
}

void Render_Component::Stamp(SDL_Rect spritesheet_clip, SDL_Rect pos_rect, int tile_offset_x, int tile_offset_y)
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet, dedicated_multisprite_num, spritesheet_clip, pos_rect);
}

void Render_Component::Clear_Sprite()
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet, dedicated_multisprite_num, { 0,0,0,0 }, { 0,0,0,0 }, true);
}

void Render_Component::Build_Wall_Multisprite()
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

void Render_Component::Build_Wall_Multisprite_Handle_Exterior_Walls(int num_space_walls, int num_surrounding_walls)
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

void Render_Component::Build_Wall_Multisprite_Handle_Simple_Interior_Walls(int num_surrounding_walls)
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

void Render_Component::Build_Wall_Multisprite_Handle_Complex_Exterior_Walls(int num_surrounding_walls, int num_diagonal_vacuum)
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

void Render_Component::Build_Floor_Multisprite()
{

}

// Animation Commands
void Render_Component::Increment_Structure_Animation_Frame()
{
	structure_animation_frame++;
	if (structure_animation_frame >= max_structure_animation_frames)
	{
		structure_animation_frame = 0;
	}
}
void Render_Component::Decrement_Structure_Animation_Frame()
{
	structure_animation_frame--;
	if (structure_animation_frame < 0)
	{
		structure_animation_frame = max_structure_animation_frames - 1;
	}
}