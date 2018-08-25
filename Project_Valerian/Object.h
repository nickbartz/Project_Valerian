#pragma once
#include<Render_Component.h>

class SDL_Renderer;
class Service_Locator;

class Object
{
public:
	Object(Service_Locator* service_locator = NULL, int x_grid = 0, int y_grid =0, int tile_width = 0, int tile_height = 0);

	void Draw(SDL_Rect camera);
	void Draw_From_Instance(SDL_Rect pos_rect);

	void Assign_Simple_Clip_Renderer(int spritesheet, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	void free();

private:
	Service_Locator* service_locator;

	int grid_x = 0;
	int grid_y = 0;
	int tile_width = 0;
	int tile_height = 0;
	
	// Render Components
	Core_Render * core_render = NULL;
	Simple_Clip_Render * simple_clip = NULL;
	Multi_Clip_Render * multi_clip = NULL;

	// AI Components



	// Physics Components
};

