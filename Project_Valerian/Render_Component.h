#pragma once

#include<SDL.h>
#include<Adjacent_Type_Array.h>

class Global_Service_Locator;
class Object_Service_Locator;
struct Structure_Template;
struct Entity_Template;
class Custom_Message;

struct Animation_State
{
	SDL_Rect component_clip = {0,0,0,0};
	int component_current_frame = 0;
	int component_max_frames = 0;
};

class Render_Component
{
public:
	// Generic Init
	Render_Component(Global_Service_Locator* service_locator, Object_Service_Locator* oLocator, int object_type, int object_template_id);

	// Init for structures
	Render_Component(Global_Service_Locator* service_locator, Object_Service_Locator* oLocator, Structure_Template object_config, Adjacent_Structure_Array neighbors);

	// Init for entities
	Render_Component(Global_Service_Locator* service_locator, Object_Service_Locator* oLocator, Entity_Template entity_config);

	void Update();

	void Draw(SDL_Rect pos_rect);
	void Draw_With_Background_Renderer(SDL_Rect pos_rect);
	void Draw_With_Simple_Clip(SDL_Rect pos_rect);
	void Draw_With_Multi_Clip(SDL_Rect pos_rect);
	void Draw_With_Animated_Simple_Clip(SDL_Rect pos_rect);

	void Draw_With_Complex_Entity_Animation(SDL_Rect pos_rect);
	void Draw_Entity_Animation_Component(SDL_Rect pos_rect, SDL_Rect sprite_clip, int animation_frame, int spritesheet);

	void Draw_Overlays(SDL_Rect pos_rect);

	// Overlay Specific Functions
	void Handle_Oxygenation_Overlay(SDL_Rect pos_rect, int oxygenation_level);

	void Initialize_Dedicated_Multisprite();
	void Deinitialize_Dedicated_Multisprite();
	bool Is_Init();

	// Update Functions
	void Update_Neighbor_Array(Adjacent_Structure_Array new_neighbors);
	void Adjust_Multisprite_To_Surroundings();
	void Check_For_Messages(int grid_x, int grid_y);
	void Check_Tile_Update_Message(Custom_Message* tile_update);

	// function for different multiclip types
	void Clear_Sprite();
	void Stamp(SDL_Rect spritesheet_clip, SDL_Rect pos_rect, int tile_offset_x = 0, int tile_offset_y = 0);
	void Build_Wall_Multisprite(); 
	void Handle_Upper_Left_Wall_Quad(int sprite_offset_x, int sprite_offset_y);
	void Handle_Upper_Right_Wall_Quad(int sprite_offset_x, int sprite_offset_y);
	void Handle_Bottom_Left_Wall_Quad(int sprite_offset_x, int sprite_offset_y);
	void Handle_Bottom_Right_Wall_Quad(int sprite_offset_x, int sprite_offset_y);
	void Build_Floor_Multisprite();
	void Adjust_Door_Orientation();

	// Animation Commands
	void Increment_Simple_Animation();
	void Change_Simple_Animation(int new_animation);

	void Increment_Entity_Animation();
	void Change_Entity_Current_Animation(int new_animation);

private:
	bool init = false;
	Global_Service_Locator * service_locator;
	Object_Service_Locator* object_locator;

	// GENERIC VARIABLES
	int render_component;
	int spritesheet;
	int max_animation_frames;
	int current_animation_frame = 0;
	int animation_direction = 1;
	int simple_animation_type = SIMPLE_ANIMATION_NULL;

	// STRUCTURE_SPECIFIC VARIABLES
	int multiclip_type;
	int dedicated_multisprite_num;
	SDL_Rect sprite_clip;
	SDL_Rect sprite_coords;
	Adjacent_Structure_Array neighbors;
	int orientation_y_clip_offset = 0; // 0 is horizontal, 1 is vertical

	// ENTITY SPECIFIC VARIABLES
	Animation_State entity_animations[MAX_NUM_ANIMATIONS][MAX_NUM_COMPONENTS];
	int num_entity_animations = 0;
	int num_entity_components = 0;
	int current_entity_anim = ANIM_WALK_DOWN;
};