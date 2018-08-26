#pragma once
#include<Global_Constants.h>
#include<Render_Component.h>
#include<AI_Component.h>

class SDL_Renderer;
class Service_Locator;

class Object
{
public:
	Object(SDL_Rect location = { 0,0,0,0 }, Service_Locator* service_locator = NULL);

	void Draw(SDL_Rect camera, SDL_Rect pos_rect);

	void Assign_Simple_Clip_Tile_Renderer(Service_Locator* service_locator, SDL_Rect tile_specs, int spritesheet, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void Assign_Background_Renderer(Service_Locator* service_locator, int spritesheet, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	void free();

private:
	Service_Locator* service_locator;

	int object_type = OBJECT_UNASSIGNED;

	SDL_Rect object_pos;
	
	// Core Render Component
	Core_Render * core_render = NULL;

	// Optional Render Components
	Simple_Clip_Tile_Renderer * simple_tile_clip = NULL;
	Multi_Clip_Tile_Renderer * tile_multi_clip = NULL;
	Background_Renderer * background_clip = NULL;

	// Core AI Component
	Core_AI_Component* core_AI_component = NULL;

	// Physics Components


};

