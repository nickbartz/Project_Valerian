#include<Draw_System.h>
#include<Service_Locator.h>
#include<Render_Component.h>
#include<iostream>
#include<SDL.h>
#include<Cursor.h>
#include<algorithm>
#include<Game_Library.h>
#include<AI_Stats_Component.h>>
#include<AI_Movement_Component.h>
#include<Scene_Graph.h>

using namespace std;

// Generic Init
Render_Component::Render_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int object_type, int object_template_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	switch (object_type)
	{
	case OBJECT_TYPE_SCAFFOLD:
		{Structure_Template * object_config = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(object_template_id);
		render_component = object_config->render_component_type;
		multiclip_type = object_config->multiclip_type;
		spritesheet = object_config->spritesheet;
		is_scaffold = true;
		override_color = { 100,255,255,100 };

		sprite_clip = object_config->tile_clip;
		sprite_coords = object_config->tile_specs;

		max_animation_frames = object_config->num_animation_frame;
		neighbors = service_locator->get_Scene_Graph()->Return_Neighboring_Tiles(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord());

		// If the object is a multiclip, handle multiclip
		if (render_component == RENDER_COMPONENT_MULTICLIP)
		{
			Initialize_Dedicated_Multisprite();
			Adjust_Multisprite_To_Surroundings();
		}
		// If the object is a door, handle door
		else if (object_config->structure_type == 8)
		{
			Adjust_Door_Orientation();
		}
		}
		break;
	case OBJECT_TYPE_ENTITY:
		{Entity_Template* entity_config = service_locator->get_Game_Library()->Fetch_Entity_Template(object_template_id);
		render_component = entity_config->render_component;
		num_entity_animations = entity_config->num_entity_animations;
		num_entity_components = entity_config->num_entity_components;

		for (int i = 0; i < entity_config->num_entity_animations; i++)
		{
			for (int p = 0; p < entity_config->num_entity_components; p++)
			{
				entity_animations[i][p] = entity_config->entity_animations[i][p];
			}
		}
		}
		break;
	case OBJECT_TYPE_PROJECTILE:
		{if (service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->is_point_not_sprite == 1) render_component = RENDER_COMPONENT_POINT_RENDER;
		else if (service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->render == 1) render_component = RENDER_COMPONENT_ANIMATED_CLIP;
		else render_component = RENDER_COMPONENT_NONE;
		simple_animation_type = SIMPLE_ANIMATION_BACK_AND_FORTH;
		spritesheet = SPRITESHEET_PROJECTILE;
		sprite_clip = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->sprite_clip;
		max_animation_frames = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->num_animation_frames;
		max_animation_delay = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->animation_delay;
		override_color = service_locator->get_Game_Library()->Fetch_Projectile_Template(object_template_id)->projectile_color;
		}
		break;
	case OBJECT_TYPE_CONTAINER:
		{render_component = RENDER_COMPONENT_SIMPLECLIP;
		spritesheet = SPRITESHEET_ICON;
		sprite_clip = service_locator->get_Game_Library()->Fetch_Item_Template(object_template_id)->sprite_specs;
		break;
		}
	}
}

// Init for Structures
Render_Component::Render_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator,  Structure_Template object_config, Adjacent_Structure_Array neighbor_array)
{
	service_locator = sLocator;
	object_locator = oLocator;

	render_component = object_config.render_component_type;
	multiclip_type = object_config.multiclip_type;
	spritesheet = object_config.spritesheet;

	sprite_clip = object_config.tile_clip;
	sprite_coords = object_config.tile_specs;

	max_animation_frames = object_config.num_animation_frame;
	neighbors = neighbor_array;

	// If the object is a multiclip, handle multiclip
	if (render_component == RENDER_COMPONENT_MULTICLIP)
	{
		Initialize_Dedicated_Multisprite();
		Adjust_Multisprite_To_Surroundings();
	}
	// If the object is a door, handle door
	else if (object_config.structure_type == 8)
	{
		Adjust_Door_Orientation();
	}
}

void Render_Component::Update()
{
	if (render_component == RENDER_COMPONENT_ANIMATED_CLIP)
	{
		Increment_Simple_Animation();
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
	case RENDER_COMPONENT_POINT_RENDER:
		Draw_Point(pos_rect);
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

	SDL_Rect new_clip = { sprite_clip.x, sprite_clip.y + orientation_y_clip_offset * SPRITE_SIZE, sprite_clip.w, sprite_clip.h };

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, new_clip, 0.0, NULL, SDL_FLIP_NONE, override_color);

	// If the sprite is several stories high, the 2nd story needs to be printed seperately and later so that i will appear to float above any people walking around so they appear to go behind it
	if (sprite_coords.h == 2 && spritesheet == SPRITESHEET_MID_1)
	{
		// As a first step, change the clip to the 2nd story of the sprite on the sprite sheet 
		SDL_Rect new_clip = { sprite_clip.x, sprite_clip.y - SPRITE_SIZE + orientation_y_clip_offset, sprite_clip.w, sprite_clip.h };

		// Now re-do the draw rect and send a new instruction to the draw system to draw that 2nd story 
		draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, ((pos_rect.y - TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(SPRITESHEET_MID_2, draw_rect, new_clip, 0.0, NULL, SDL_FLIP_NONE, override_color);
	}
}

void Render_Component::Draw_With_Multi_Clip(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, sprite_coords.w*camera.w, sprite_coords.h*camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Multisprite_Render_Job_To_Render_Cycle(spritesheet, dedicated_multisprite_num, draw_rect, 0.0, NULL, SDL_FLIP_NONE, override_color);
}

void Render_Component::Draw_With_Animated_Simple_Clip(SDL_Rect pos_rect)
{	
	
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect anim_clip = { sprite_clip.x + current_animation_frame * SPRITE_SIZE, sprite_clip.y + orientation_y_clip_offset*SPRITE_SIZE, sprite_clip.w, sprite_clip.h };

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, anim_clip, 0.0, NULL, SDL_FLIP_NONE, override_color);

	// If the sprite is several stories high, the 2nd story needs to be printed seperately and later so that i will appear to float above any people walking around so they appear to go behind it
	if (sprite_coords.h == 2 && spritesheet == SPRITESHEET_MID_1)
	{
		// As a first step, change the clip to the 2nd story of the sprite on the sprite sheet 
		SDL_Rect new_clip = { sprite_clip.x + current_animation_frame, sprite_clip.y - SPRITE_SIZE, sprite_clip.w, sprite_clip.h };

		// Now re-do the draw rect and send a new instruction to the draw system to draw that 2nd story 
		draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, ((pos_rect.y - TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(SPRITESHEET_MID_2, draw_rect, new_clip, 0.0, NULL, SDL_FLIP_NONE, override_color);
	}
}

void Render_Component::Draw_With_Complex_Entity_Animation(SDL_Rect pos_rect)
{
	for (int i = 0; i < num_entity_components; i++)
	{
		Draw_Entity_Animation_Component(pos_rect, entity_animations[current_entity_anim][i].component_clip, entity_animations[current_entity_anim][i].component_current_frame, SPRITESHEET_ENTITY);
	}
}

void Render_Component::Draw_Point(SDL_Rect pos_rect)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(-1, draw_rect, true, override_color, PRIMITIVE_TYPE_POINT);
}

// Draw function sub-functions

void Render_Component::Draw_Entity_Animation_Component(SDL_Rect pos_rect, SDL_Rect sprite_clip, int animation_frame, int spritesheet)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect anim_clip = { sprite_clip.x + animation_frame * SPRITE_SIZE, sprite_clip.y, sprite_clip.w, sprite_clip.h };

	// Adjust the draw rectangle by the camera position and camera zoom
	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet, draw_rect, anim_clip, 0.0, NULL, SDL_FLIP_NONE, override_color);
}

void Render_Component::Draw_Overlays(SDL_Rect pos_rect)
{
	AI_Stats_Component* stats_pointer = object_locator->Return_AI_Stats_Pointer();
	int current_health = stats_pointer->Return_Stat_Value(STAT_OBJECT_HEALTH);
	int max_health = stats_pointer->Return_Stat_Value(STAT_OBJECT_MAX_HEALTH);
	int built_level = stats_pointer->Return_Stat_Value(STAT_STRUCTURE_BUILT_LEVEL);
	int max_built_level = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(stats_pointer->Return_Template_ID())->max_built_level;

	// Draw Oxygenation Level
	if (stats_pointer->Return_Object_Type() == OBJECT_TYPE_STRUCTURE)
	{
		int oxygen_level = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_STRUCTURE_OXYGEN_LEVEL);
		if (oxygen_level > 0) Handle_Oxygenation_Overlay(pos_rect, oxygen_level);
	}

	// Draw Current Health
	if ( current_health < max_health && current_health > 0)
	{
		Handle_Progress_Overlays(pos_rect, { 255,0,100,255 }, 5, current_health, max_health);
	}

	if (built_level < max_built_level)
	{
		Handle_Progress_Overlays(pos_rect, { 100,100,255,255 }, 11, built_level, max_built_level);
	}


	//Coordinate object_location = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
	//string x_coord = to_string(object_location.x);
	//string y_coord = to_string(object_location.y);
	//x_coord.push_back(',');
	//for (int i = 0; i < y_coord.size(); i++)
	//{
	//	x_coord.push_back(y_coord[i]);
	//}
	//service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, service_locator->get_Cursor_Pointer()->Convert_World_Rect_To_Screen_Rect(object_locator->Return_AI_Movement_Pointer()->Return_World_Pos()), false, { 255,255,255,255 }, PRIMITIVE_TYPE_RECT);
	//service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle({ 1, service_locator->get_Cursor_Pointer()->Convert_World_Rect_To_Screen_Rect(object_locator->Return_AI_Movement_Pointer()->Return_World_Pos()), FONT_SMALL_BOLD,x_coord, {0,255,255,255} });
}

void Render_Component::Handle_Oxygenation_Overlay(SDL_Rect pos_rect, int oxygenation_level)
{
	SDL_Rect camera = service_locator->get_Cursor_Pointer()->Get_Camera();

	SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };

	SDL_Color overlay_color = { 0,0,255,oxygenation_level * 5 };

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, draw_rect, true, overlay_color);
}

void Render_Component::Handle_Progress_Overlays(SDL_Rect pos_rect, SDL_Color bar_color, int height_offset, int current_health, int max_health)
{
	SDL_Rect progress_bar_Rect = { pos_rect.x, pos_rect.y - height_offset, current_health*TILE_SIZE/max_health, height_offset };
	SDL_Rect draw_rect = service_locator->get_Cursor_Pointer()->Convert_World_Rect_To_Screen_Rect(progress_bar_Rect);

	service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, draw_rect, true, bar_color, PRIMITIVE_TYPE_RECT);
	if (draw_rect.w > 0) service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, draw_rect, false, { 255,255,255,255 }, PRIMITIVE_TYPE_RECT);
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

void Render_Component::Adjust_Door_Orientation()
{
	Game_Library* gl = service_locator->get_Game_Library();

	if (gl->is_wall(neighbors.asa[1][0][1]) && gl->is_wall(neighbors.asa[1][2][1]))
	{
		orientation_y_clip_offset = 0;
	}
	else if (gl->is_wall(neighbors.asa[1][1][0]) && gl->is_wall(neighbors.asa[1][1][2]))
	{
		orientation_y_clip_offset = 1;
	}
}

void Render_Component::Check_For_Messages(int grid_x, int grid_y)
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		if (service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(0) == MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION )
		{
			Check_Tile_Update_Message(&service_locator->get_MB_Pointer()->Custom_Message_Array[i]);
		}
	}
}

void Render_Component::Check_Tile_Update_Message(Custom_Message* tile_update)
{
	Coordinate grid_point = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
	int grid_x = grid_point.x;
	int grid_y = grid_point.y;
	
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
	SDL_Rect offset_clip = { spritesheet_clip.x + tile_offset_x, spritesheet_clip.y + tile_offset_y, spritesheet_clip.w, spritesheet_clip.h };
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet, dedicated_multisprite_num, offset_clip, pos_rect);
}

void Render_Component::Clear_Sprite()
{
	service_locator->get_Draw_System_Pointer()->Stamp_Sprite_Onto_Multisprite(spritesheet, dedicated_multisprite_num, { 0,0,0,0 }, { 0,0,0,0 }, true);
}

void Render_Component::Build_Wall_Multisprite()
{
	Clear_Sprite();
	
	Handle_Upper_Left_Wall_Quad(sprite_clip.x, sprite_clip.y);

	Handle_Upper_Right_Wall_Quad(sprite_clip.x, sprite_clip.y);

	Handle_Bottom_Left_Wall_Quad(sprite_clip.x, sprite_clip.y);

	Handle_Bottom_Right_Wall_Quad(sprite_clip.x, sprite_clip.y);
}

void Render_Component::Handle_Upper_Left_Wall_Quad(int sprite_offset_x, int sprite_offset_y)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TW = TILE_SIZE;
	int TQ = TILE_SIZE / 2;
	int cto_x = sprite_offset_x;
	int cto_y = sprite_offset_y;

	bool left_space = false;
	bool left_wall = false;
	bool left_floor = false;
	bool top_space = false;
	bool top_wall = false;
	bool top_floor = false;
	bool top_left_space = false;
	bool top_left_wall = false;
	bool top_left_floor = false;
	
	if (gl->is_null(neighbors.asa[0][0][1]) && gl->is_null(neighbors.asa[1][0][1])) left_space = true;
	if (gl->is_wall(neighbors.asa[1][0][1])) left_wall = true;
	if (gl->is_floor(neighbors.asa[0][0][1]) && !gl->is_wall(neighbors.asa[1][0][1])) left_floor = true;

	if (gl->is_null(neighbors.asa[0][1][0]) && gl->is_null(neighbors.asa[1][1][0])) top_space = true;
	if (gl->is_wall(neighbors.asa[1][1][0])) top_wall = true;
	if (gl->is_floor(neighbors.asa[0][1][0]) && !gl->is_wall(neighbors.asa[1][1][0])) top_floor = true;

	if (gl->is_null(neighbors.asa[0][0][0]) && gl->is_null(neighbors.asa[1][0][0])) top_left_space = true;
	if (gl->is_wall(neighbors.asa[1][0][0])) top_left_wall = true;
	if (gl->is_floor(neighbors.asa[0][0][0]) && !gl->is_wall(neighbors.asa[1][0][0])) top_left_floor = true;

	if (left_space && top_space) Stamp({ 0 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_space && top_wall) Stamp({ 2 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && top_wall) Stamp({ 5 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && top_wall && top_left_floor) Stamp({ 6 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && top_wall && top_left_space) Stamp({ 8 * TQ,1 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && top_wall && top_left_wall) Stamp({ 12 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && top_space) Stamp({ 3 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && top_floor) Stamp({ 4 * TQ,1 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && top_floor) Stamp({ 11 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && top_space) Stamp({ 0 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	else if (left_space && top_floor) Stamp({ 0 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);

	// Check if there's an exterior door
	if (gl->is_door(neighbors.asa[1][0][1]) && gl->is_null(neighbors.asa[0][0][2]) && gl->is_null(neighbors.asa[1][0][2]))
	{
		Stamp({ 14 * TQ,2 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
	}
	else if (gl->is_door(neighbors.asa[1][1][0]) && gl->is_null(neighbors.asa[0][2][1]) && gl->is_null(neighbors.asa[1][2][1])) Stamp({ 14 * TQ,0 * TQ,TQ,TQ }, { 0,0,TQ,TQ }, cto_x, cto_y);
}
void Render_Component::Handle_Upper_Right_Wall_Quad(int sprite_offset_x, int sprite_offset_y)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TW = TILE_SIZE;
	int TQ = TILE_SIZE / 2;
	int cto_x = sprite_offset_x;
	int cto_y = sprite_offset_y;

	bool right_space = false;
	bool right_wall = false;
	bool right_floor = false;
	bool top_space = false;
	bool top_wall = false;
	bool top_floor = false;
	bool top_right_space = false;
	bool top_right_wall = false;
	bool top_right_floor = false;

	if (gl->is_null(neighbors.asa[0][2][1]) && gl->is_null(neighbors.asa[1][2][1])) right_space = true;
	if (gl->is_wall(neighbors.asa[1][2][1])) right_wall = true;
	if (gl->is_floor(neighbors.asa[0][2][1]) && !gl->is_wall(neighbors.asa[1][2][1])) right_floor = true;

	if (gl->is_null(neighbors.asa[0][1][0]) && gl->is_null(neighbors.asa[1][1][0])) top_space = true;
	if (gl->is_wall(neighbors.asa[1][1][0])) top_wall = true;
	if (gl->is_floor(neighbors.asa[0][1][0]) && !gl->is_wall(neighbors.asa[1][1][0])) top_floor = true;

	if (gl->is_null(neighbors.asa[0][2][0]) && gl->is_null(neighbors.asa[1][2][0])) top_right_space = true;
	if (gl->is_wall(neighbors.asa[1][2][0])) top_right_wall = true;
	if (gl->is_floor(neighbors.asa[0][2][0]) && !gl->is_wall(neighbors.asa[1][2][0])) top_right_floor = true;

	if (right_space && top_space) Stamp({ 1 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_space && top_wall) Stamp({ 3 * TQ,1 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && top_wall) Stamp({ 4 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && top_wall && top_right_floor) Stamp({ 7 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && top_wall && top_right_space) Stamp({ 9 * TQ,1 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && top_wall && top_right_wall) Stamp({ 12 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && top_space) Stamp({ 3 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && top_floor) Stamp({ 4 * TQ,1 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && top_floor) Stamp({ 10 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && top_space) Stamp({ 1 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	else if (right_space && top_floor) Stamp({ 1 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);

	// Check if there's an exterior door
	if (gl->is_door(neighbors.asa[1][2][1]) && gl->is_null(neighbors.asa[0][2][2]) && gl->is_null(neighbors.asa[1][2][2]))
	{
		Stamp({ 15 * TQ,2 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
	}
	else if (gl->is_door(neighbors.asa[1][1][0]) && gl->is_null(neighbors.asa[0][0][0]) && gl->is_null(neighbors.asa[1][0][0])) Stamp({ 15 * TQ,0 * TQ,TQ,TQ }, { TQ,0,TQ,TQ }, cto_x, cto_y);
}
void Render_Component::Handle_Bottom_Left_Wall_Quad(int sprite_offset_x, int sprite_offset_y)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TW = TILE_SIZE;
	int TQ = TILE_SIZE / 2;
	int cto_x = sprite_offset_x;
	int cto_y = sprite_offset_y;

	bool left_space = false;
	bool left_wall = false;
	bool left_floor = false;
	bool bottom_space = false;
	bool bottom_wall = false;
	bool bottom_floor = false;
	bool bottom_left_space = false;
	bool bottom_left_wall = false;
	bool bottom_left_floor = false;

	if (gl->is_null(neighbors.asa[0][0][1]) && gl->is_null(neighbors.asa[1][0][1])) left_space = true;
	if (gl->is_wall(neighbors.asa[1][0][1])) left_wall = true;
	if (gl->is_floor(neighbors.asa[0][0][1]) && !gl->is_wall(neighbors.asa[1][0][1])) left_floor = true;

	if (gl->is_null(neighbors.asa[0][1][2]) && gl->is_null(neighbors.asa[1][1][2])) bottom_space = true;
	if (gl->is_wall(neighbors.asa[1][1][2])) bottom_wall = true;
	if (gl->is_floor(neighbors.asa[0][1][2]) && !gl->is_wall(neighbors.asa[1][1][2])) bottom_floor = true;

	if (gl->is_null(neighbors.asa[0][0][2]) && gl->is_null(neighbors.asa[1][0][2])) bottom_left_space = true;
	if (gl->is_wall(neighbors.asa[1][0][2])) bottom_left_wall = true;
	if (gl->is_floor(neighbors.asa[0][0][2]) && !gl->is_wall(neighbors.asa[1][0][2])) bottom_left_floor = true;

	if (left_space && bottom_space) Stamp({ 0 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_space && bottom_wall) Stamp({ 2 * TQ,0 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && bottom_wall) Stamp({ 5 * TQ,0 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && bottom_wall && bottom_left_floor) Stamp({ 6 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && bottom_wall && bottom_left_space) Stamp({ 8 * TQ,0 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && bottom_wall && bottom_left_wall) Stamp({ 12 * TQ,0 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && bottom_space) Stamp({ 2 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_wall && bottom_floor) Stamp({ 5 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && bottom_floor) Stamp({ 11 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_floor && bottom_space) Stamp({ 0 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	else if (left_space && bottom_floor) Stamp({ 0 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);

	// Check if there's an exterior door
	if (gl->is_door(neighbors.asa[1][0][1]) && gl->is_null(neighbors.asa[0][0][0]) && gl->is_null(neighbors.asa[1][0][0]))
	{
		Stamp({ 14 * TQ,3 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);
	}
	else if (gl->is_door(neighbors.asa[1][1][2]) && gl->is_null(neighbors.asa[0][2][2]) && gl->is_null(neighbors.asa[1][2][2])) Stamp({ 14 * TQ,1 * TQ,TQ,TQ }, { 0,TQ,TQ,TQ }, cto_x, cto_y);

}
void Render_Component::Handle_Bottom_Right_Wall_Quad(int sprite_offset_x, int sprite_offset_y)
{
	Game_Library* gl = service_locator->get_Game_Library();
	int TW = TILE_SIZE;
	int TQ = TILE_SIZE / 2;
	int cto_x = sprite_offset_x;
	int cto_y = sprite_offset_y;

	bool right_space = false;
	bool right_wall = false;
	bool right_floor = false;
	bool bottom_space = false;
	bool bottom_wall = false;
	bool bottom_floor = false;
	bool bottom_right_space = false;
	bool bottom_right_wall = false;
	bool bottom_right_floor = false;

	if (gl->is_null(neighbors.asa[0][2][1]) && gl->is_null(neighbors.asa[1][2][1])) right_space = true;
	if (gl->is_wall(neighbors.asa[1][2][1])) right_wall = true;
	if (gl->is_floor(neighbors.asa[0][2][1]) && !gl->is_wall(neighbors.asa[1][2][1])) right_floor = true;

	if (gl->is_null(neighbors.asa[0][1][2]) && gl->is_null(neighbors.asa[1][1][2])) bottom_space = true;
	if (gl->is_wall(neighbors.asa[1][1][2])) bottom_wall = true;
	if (gl->is_floor(neighbors.asa[0][1][2]) && !gl->is_wall(neighbors.asa[1][1][2])) bottom_floor = true;

	if (gl->is_null(neighbors.asa[0][2][2]) && gl->is_null(neighbors.asa[1][2][2])) bottom_right_space = true;
	if (gl->is_wall(neighbors.asa[1][2][2])) bottom_right_wall = true;
	if (gl->is_floor(neighbors.asa[0][2][2]) && !gl->is_wall(neighbors.asa[1][2][2])) bottom_right_floor = true;

	if (right_space && bottom_space) Stamp({ 1 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_space && bottom_wall) Stamp({ 3 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && bottom_wall) Stamp({ 4 * TQ,0 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && bottom_wall && bottom_right_floor) Stamp({ 7 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && bottom_wall && bottom_right_space) Stamp({ 9 * TQ,0 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && bottom_wall && bottom_right_wall) Stamp({ 12 * TQ,0 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && bottom_space) Stamp({ 2 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_wall && bottom_floor) Stamp({ 5 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && bottom_floor) Stamp({ 10* TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_floor && bottom_space) Stamp({ 1 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	else if (right_space && bottom_floor) Stamp({ 1 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);

	// Check if there's an exterior door
	if (gl->is_door(neighbors.asa[1][2][1]) && gl->is_null(neighbors.asa[0][2][0]) && gl->is_null(neighbors.asa[1][2][0]))
	{
		Stamp({ 15 * TQ,3 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
	}
	else if (gl->is_door(neighbors.asa[1][1][2]) && gl->is_null(neighbors.asa[0][0][2]) && gl->is_null(neighbors.asa[1][0][2])) Stamp({ 15 * TQ,1 * TQ,TQ,TQ }, { TQ,TQ,TQ,TQ }, cto_x, cto_y);
}

void Render_Component::Build_Floor_Multisprite()
{

}

// Animation Commands
void Render_Component::Increment_Simple_Animation()
{
	if (animation_delay == 0)
	{
		switch (simple_animation_type)
		{
		case SIMPLE_ANIMATION_NULL:
			break;
		case SIMPLE_ANIMATION_INCREMENT_UNTIL_COMPLETE:
			current_animation_frame++;
			if (current_animation_frame >= max_animation_frames)
			{
				current_animation_frame = max_animation_frames;
			}
			break;
		case SIMPLE_ANIMATION_DECREMENT_UNTIL_COMPLETE:
			current_animation_frame--;
			if (current_animation_frame <= 0)
			{
				current_animation_frame = 0;
			}
			break;
		case SIMPLE_ANIMATION_PAUSE:
			break;
		case SIMPLE_ANIMATION_INCREMENT_AND_REPEAT:
			current_animation_frame++;
			if (current_animation_frame >= max_animation_frames)
			{
				current_animation_frame = 0;
			}
			break;
		case SIMPLE_ANIMATION_BACK_AND_FORTH:
			if (animation_direction == 1)
			{
				current_animation_frame++;
				if (current_animation_frame >= max_animation_frames)
				{
					animation_direction = -1;
				}
			}
			else if (animation_direction = -1)
			{
				current_animation_frame--;
				if (current_animation_frame <= 0)
				{
					animation_direction = 1;
				}
			}
			break;
		}
		animation_delay = max_animation_delay;
	}
	else
	{
		animation_delay--;
		if (animation_delay < 0) animation_delay = max_animation_delay;
	}
}
void Render_Component::Increment_Entity_Animation()
{
	for (int i = 0; i < num_entity_components; i++)
	{
		entity_animations[current_entity_anim][i].component_current_frame++;
		if (entity_animations[current_entity_anim][i].component_current_frame >= entity_animations[current_entity_anim][i].component_max_frames)
		{
			entity_animations[current_entity_anim][i].component_current_frame = 0;
		}
	}
}
void Render_Component::Change_Simple_Animation(int new_animation)
{
	simple_animation_type = new_animation;
}
void Render_Component::Change_Entity_Current_Animation(int new_animation)
{
	current_entity_anim = new_animation;
}