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
#include<Service_Locator.h>

class Draw_System
{
public:
	Draw_System(Service_Locator* service_locator, FC_Font* font_array_start[], Uint32 window_format);
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

	// Multisprite functions
	int Add_New_Spritesheet_To_Multisprite(int spritesheet_num, SDL_Renderer* game_renderer);
	void Stamp_Sprite_Onto_Multisprite(int spritesheet_num, int multisprite_num, SDL_Rect clip);
	void Clear_Multisprite();
	void Remove_Multisprite(int spritesheet_num, int multisprite_num);

	// Functions to Add or Remove From the Draw Array
	void Add_Sprite_Render_Job_To_Render_Cycle(int spritesheet, SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);
	void Add_Multisprite_Render_Job_To_Render_Cycle(int spritesheet_num, int multi_tile_num, SDL_Rect pos_rect);

	void Add_Primitive_To_Render_Cycle(Primitive_Instruction primitive);
	void Clear_Primitive_Instruction_Array();

	void Add_Text_Job_To_Render_Cycle(Text_Instruction string);
	void Clear_Text_Instruction_Array();

	// Functions to Draw, called by main
	void Draw(SDL_Renderer* render_target);

	void Draw_Spritesheet_Directly(SDL_Renderer* render_target, int spritesheet_num, SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);
	void Draw_Sprites(SDL_Renderer* render_target);
	void Draw_Primitives(SDL_Renderer* render_target);
	void Draw_Text_Strings(SDL_Renderer* render_target);

	// Free
	void free();

private:
	Service_Locator * service_locator;

	int count_num_fonts = 0;

	int count = 0;

	// Window Format
	Uint32 window_format;

	// Font Array
	FC_Font* font_array[MAX_NUM_FONTS];

	// Multisprites
	int current_num_base_multisprites;
	Spritesheet base_multisprite[MAX_SPRITES_PER_MULTISPRITE]; // composite sprites built up of smaller parts
	int current_num_mid_multisprites;
	Spritesheet mid_multisprite[MAX_SPRITES_PER_MULTISPRITE];

	//Spritesheets
	Spritesheet background_spritesheet;
	Spritesheet base_spritesheet; // simple tile_based sprites 
	Spritesheet base_overlay; // overlay of simple tile based sprites 
	Spritesheet mid_spritesheet;
	Spritesheet mid_overlay_spritesheet;
	Spritesheet mid_2_spritesheet;
	Spritesheet entity;

	// Primitive array 
	int count_num_primitives = -1;
	Primitive_Instruction Primitive_Instruction_Array[MAX_NUM_PRIMITIVES];

	// Text print array
	int count_num_print_text = -1;
	Text_Instruction Text_Instruction_Array[MAX_NUM_TEXT_PRINT];

};