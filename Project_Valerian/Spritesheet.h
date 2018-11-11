#pragma once

using namespace std;
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<Global_Constants.h>

class Spritesheet
{
public:
	Spritesheet();

	void Init(string path, SDL_Renderer* game_renderer, Uint32 window_format);
	void Init_As_Multisprite(SDL_Renderer* game_renderer, int multisprite_width = TILE_SIZE, int multisprite_height = TILE_SIZE);
	void Initialize_Prebaked_Texture_Layer(SDL_Renderer* game_renderer, int width, int height);
	void Deinitialize();

	bool is_init();

	struct Sprite_Instructions
	{
		SDL_Rect position_rect = { 0,0,0,0 };
		SDL_Rect clip_rect = { 0,0,0,0 };
		double angle = 0.0;
		SDL_Point* center = NULL;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		SDL_Color color_shift = { 255,255,255,255 };
	};

	void Draw(SDL_Renderer* game_renderer, bool use_prebaked = false);
	void Draw_Directly(SDL_Renderer* game_renderer, SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);
	void Add_Sprite_Instructions(SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE, SDL_Color color_shift = { 255,255,255,255 });

	bool Get_Prebake_Init();
	void Set_Prebake_Init(bool init);
	void Draw_Sprite_Instructions_To_Texture(SDL_Renderer* game_renderer);
	void Draw_Prebaked_Texture(SDL_Renderer* game_renderer);

	void Set_SDL_Texture_As_Render_Target(SDL_Renderer* game_renderer);
	void Sprite_Clear_Instructions();

	// No need for clear sprite functions since draw automatically flushes the array when complete

	void free();

	int return_uniq_id();

private:
	int uniq_id;
	bool init = false;
	bool prebaked_layer_init = false;
	int current_num_instructions = 0;
	Sprite_Instructions instruction_array[MAX_SPRITE_INSTRUCTIONS];
	int spritesheet_w;
	int spritesheet_h;

	SDL_Texture* spritesheet_texture;

	SDL_Texture* prebaked_layer_texture;

};
