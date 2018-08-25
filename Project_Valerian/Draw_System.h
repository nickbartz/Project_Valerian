#pragma once

using namespace std;
#include<Global_Constants.h>
#include<iostream>
#include<string>
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_FontCache.h>
#include <stdio.h>
#include <Spritesheet.h>

class Draw_System
{
public:
	Draw_System(SDL_Renderer* game_renderer, FC_Font* font_array_start[]);
	void Init_Sprites(SDL_Renderer* game_renderer);

	// The standard primitive rectangle that will be used to instruct the renderer
	struct Primitive_Instruction
	{
		int init = 0;
		SDL_Rect pos_rect = { 0,0,0,0 };
		bool filled = false;
		SDL_Color primitive_color = { 255,255,255,255 };
	};

	struct Text_Instruction
	{
		int init = 0;
		SDL_Rect pos_rect = { 0,0,0,0 };
		int font = FONT_DEFAULT;
		string text_string = "test";
		SDL_Color text_color = { 255,255,255,255 };
	};

	// Query Functions
	int Return_Text_Width(int font_type, string text);

	// Functions to Add or Remove From the Draw Array
	void Add_Sprite_Render_Job_To_Render_Cycle(int spritesheet, SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);

	void Add_Primitive_To_Render_Cycle(Primitive_Instruction primitive);
	void Clear_Primitive_Instruction_Array();

	void Add_Text_Job_To_Render_Cycle(Text_Instruction string);
	void Clear_Text_Instruction_Array();

	// Functions to Draw, called by main
	void Draw(SDL_Renderer* render_target);
	void Draw_Sprites(SDL_Renderer* render_target);
	void Draw_Primitives(SDL_Renderer* render_target);
	void Draw_Text_Strings(SDL_Renderer* render_target);

	// Free
	void free();

private:

	int count_num_fonts = 0;

	// Font Array
	FC_Font* font_array[MAX_NUM_FONTS];

	//Spritesheets
	Spritesheet background_spritesheet;
	Spritesheet base_spritesheet;

	// Primitive array 
	int count_num_primitives = -1;
	Primitive_Instruction Primitive_Instruction_Array[MAX_NUM_PRIMITIVES];

	// Text print array
	int count_num_print_text = -1;
	Text_Instruction Text_Instruction_Array[MAX_NUM_TEXT_PRINT];

};