#pragma once

#include<SDL.h>
class Service_Locator;

class Core_Render
{
public:
	Core_Render(Service_Locator* service_locator = NULL);
	void Draw();

protected:
	Service_Locator * service_locator = NULL;
};

class Background_Renderer: public Core_Render
{
public :
	Background_Renderer(Service_Locator* service_locator, int sheet_num, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) : Core_Render(service_locator)
	{
		spritesheet_num = sheet_num;
		sprite_clip = clip;
		angle = ang;
		center = point;
	}

	void Draw(SDL_Rect pos_rect);

private:
	int spritesheet_num;
	SDL_Rect sprite_clip;
	double angle;
	SDL_RendererFlip flip;
	SDL_Point* center = NULL;
};

class Simple_Clip_Tile_Renderer :public Core_Render
{
public:
	Simple_Clip_Tile_Renderer(SDL_Rect tspecs, Service_Locator* service_locator, int sheet_num, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) :Core_Render(service_locator)
	{
		spritesheet_num = sheet_num;
		sprite_clip = clip;
		angle = ang;
		center = point;
		tile_specs = tspecs;
	}

	void Draw();

private:
	SDL_Rect tile_specs;
	int spritesheet_num;
	SDL_Rect sprite_clip;
	double angle;
	SDL_RendererFlip flip;
	SDL_Point* center = NULL;
};

class Multi_Clip_Tile_Renderer :public Core_Render
{
public:
	Multi_Clip_Tile_Renderer(Service_Locator* service_locator = NULL, int tile_x = 0, int tile_y = 0) : Core_Render(service_locator)
	{
	}

	void Check_Bus_For_Surrounding_Tile_Updates();
	void Draw();

private:
	// 1st array vector is the tile layer, 2nd is x_diff, 3rd is y_diff, and the value is the tile_type
	int Surrounding_Tile_Array[3][3][3];
	int grid_x;
	int grid_y;

};

class Basic_Entity_Renderer :public Core_Render
{
public:
	Basic_Entity_Renderer(Service_Locator* service_locator) :Core_Render(service_locator)
	{

	}

	void Draw(SDL_Rect pos_rect);

private:

};