#pragma once

#include<SDL.h>
#include<Adjacent_Type_Array.h>

class Global_Service_Locator;
class Object_Service_Locator;
class Structure_Template;
class Entity_Template;
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
	// Init for structures
	Render_Component(Global_Service_Locator* service_locator, Object_Service_Locator* oLocator, Structure_Template object_config, Adjacent_Structure_Array neighbors);

	// Init for entities
	Render_Component(Global_Service_Locator* service_locator, Object_Service_Locator* oLocator, Entity_Template entity_config);


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
	void Build_Wall_Multisprite_Handle_Exterior_Walls(int num_space_walls, int num_surrounding_walls);
	void Build_Wall_Multisprite_Handle_Simple_Interior_Walls(int num_surrounding_walls);
	void Build_Wall_Multisprite_Handle_Complex_Exterior_Walls(int num_surrounding_walls, int num_diagonal_vacuum);
	void Build_Floor_Multisprite();

	// Structure_Animation Commands
	void Increment_Structure_Animation_Frame();
	void Decrement_Structure_Animation_Frame();

private:
	bool init = false;
	Global_Service_Locator * service_locator;
	Object_Service_Locator* object_locator;

	int render_component;
	int spritesheet;

	// STRUCTURE_SPECIFIC VARIABLES
	int multiclip_type;
	int dedicated_multisprite_num;
	int structure_animation_frame = 0;
	int max_structure_animation_frames = 0;
	SDL_Rect sprite_clip;
	SDL_Rect sprite_coords;
	Adjacent_Structure_Array neighbors;

	// ENTITY SPECIFIC VARIABLES
	Animation_State entity_animations[MAX_NUM_ANIMATIONS][MAX_NUM_COMPONENTS];
	int num_entity_animations = 0;
	int num_entity_components = 0;
	int current_entity_anim = ANIM_WALK_DOWN;
};