#pragma once

#include<SDL.h>
#include<Adjacent_Type_Array.h>

class Service_Locator;


class Core_Render
{
public:
	Core_Render(Service_Locator* service_locator = NULL);
	void Draw();

protected:
	Service_Locator * service_locator = NULL;
};

class Overlay_Renderer : public Core_Render
{
public:
	Overlay_Renderer(Service_Locator* service_locator) :Core_Render(service_locator)
	{

	}
private:
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
		tile_coords = tspecs;
	}

	void Draw(SDL_Rect pos_rect);

private:
	SDL_Rect tile_coords;
	int spritesheet_num;
	SDL_Rect sprite_clip;
	double angle;
	SDL_RendererFlip flip;
	SDL_Point* center = NULL;
};

class Multisprite_Tile_Renderer :public Core_Render
{
public:
	Multisprite_Tile_Renderer(Service_Locator* service_locator = NULL, SDL_Rect tCoords = { 0,0,0,0 }, int spritehseet_n = SPRITESHEET_BASE, int mclip_type = MULTICLIP_NONE, Adjacent_Structure_Array array = {}) : Core_Render(service_locator)
	{
		spritesheet_num = spritehseet_n;
		multi_clip_type = mclip_type;
		neighbors = array;
		tile_coords = tCoords;
		Initialize_Dedicated_Multisprite();
		Adjust_Multisprite_To_Surroundings();
	}

	void Initialize_Dedicated_Multisprite();
	void Deinitialize_Dedicated_Multisprite();
	bool Is_Init();

	void Update_Neighbor_Array(Adjacent_Structure_Array new_neighbors);
	void Adjust_Multisprite_To_Surroundings();
	void Check_Bus_For_Surrounding_Tile_Updates();
	void Draw(SDL_Rect pos_rect);

	// function for different multiclip types
	void Clear_Sprite();
	void Stamp(SDL_Rect spritesheet_clip, SDL_Rect pos_rect, int tile_offset_x = 0, int tile_offset_y = 0);
	void Build_Wall_Multisprite();
	void Build_Wall_Multisprite_Handle_Exterior_Walls(int num_space_walls, int num_surrounding_walls);
	void Build_Wall_Multisprite_Handle_Simple_Interior_Walls(int num_surrounding_walls);
	void Build_Wall_Multisprite_Handle_Complex_Exterior_Walls(int num_surrounding_walls, int num_diagonal_vacuum);
	void Build_Floor_Multisprite();


private:
	bool init = false;
	int dedicated_multisprite_num;
	Adjacent_Structure_Array neighbors;
	SDL_Rect tile_coords;
	int multi_clip_type;
	int spritesheet_num;
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