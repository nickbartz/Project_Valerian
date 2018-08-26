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

	void Init(string path, SDL_Renderer* game_renderer);

	struct Sprite_Instructions
	{
		SDL_Rect position_rect = { 0,0,0,0 };
		SDL_Rect clip_rect = { 0,0,0,0 };
		double angle = 0.0;
		SDL_Point* center = NULL;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
	};

	void Draw(SDL_Renderer* game_renderer);

	void Add_Sprite_Instructions(SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);
	// No need for clear sprite functions since draw automatically flushes the array when complete

	void free();

private:

	int current_num_instructions = 0;
	Sprite_Instructions instruction_array[MAX_SPRITE_INSTRUCTIONS];
	int spritesheet_w;
	int spritesheet_h;

	SDL_Texture* spritesheet_texture;

};