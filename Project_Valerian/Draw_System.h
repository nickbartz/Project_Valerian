#pragma once

using namespace std;
#include<Global_Constants.h>
#include<string>
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_FontCache.h>
#include <Spritesheet.h>
#include<Service_Locator.h>
#include<Draw_Layer.h>

class Draw_System
{
public:
	Draw_System(Global_Service_Locator* service_locator, FC_Font* font_array_start[], Uint32 window_format);
	void Init_Sprites(SDL_Renderer* game_renderer);

	// The standard primitive rectangle that will be used to instruct the renderer
	struct Primitive_Instruction
	{
		int init = 0;
		SDL_Rect pos_rect = { 0,0,0,0 };
		bool filled = false;
		SDL_Color primitive_color = { 255,255,255,255 };
		int primitive_type = PRIMITIVE_TYPE_RECT;
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
	int Return_Text_Height(int font_type, string text);
	bool Return_Spritesheet_Prebaked_Status(int spritesheet);
	void Set_Spritesheet_Prebaked_Status(int spritesheet_num, bool prebake_status);
	void Set_Reset_Prebake_Status_Indicator(bool prebake_status);
	SDL_Rect Get_Offset_Rect_For_Text(SDL_Rect offset_rect, int FONT_TYPE, string text, int vertical_align = 1);

	// Multisprite functions
	int Add_New_Spritesheet_To_Multisprite(int spritesheet_num, SDL_Renderer* game_renderer);
	void Stamp_Sprite_Onto_Multisprite(int spritesheet_num, int multisprite_num, SDL_Rect clip, SDL_Rect stamp_rect, bool clear_sprite = false);
	void Remove_Multisprite(int spritesheet_num, int multisprite_num);

	// Functions to Add or Remove From the Draw Array
	void Add_Sprite_Render_Job_To_Render_Cycle(int spritesheet, SDL_Rect position_rect, SDL_Rect clip_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE, SDL_Color color_tint = {255,255,255,255});
	void Add_Multisprite_Render_Job_To_Render_Cycle(int spritesheet_num, int multi_tile_num, SDL_Rect pos_rect, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE,  SDL_Color color_shift = { 255,255,255,255 });

	void Add_Primitive_To_Render_Cycle(int init, SDL_Rect pos_rect, bool filled, SDL_Color primitive_color, int primitive_type = PRIMITIVE_TYPE_RECT);
	void Clear_Primitive_Instruction_Array();

	void Add_Text_Job_To_Render_Cycle(int init, SDL_Rect pos_rect, string text_string, int font = FONT_DEFAULT, SDL_Color text_color = { 255,255,255,255 });
	void Clear_Text_Instruction_Array();

	// Draw to layer functions
	int Get_Layer_Uniq_Id(int render_layer);
	void Draw_Sprites_To_Draw_Layer(int render_component_id, int draw_layer, int spritesheet, SDL_Rect src_rect, SDL_Rect dest_rect, bool is_multisprite = false, int multisprite_num = 0, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool has_color = false, SDL_Color override_color = { 255,255,255,255 }, bool is_primitive = false, int primitive_id = PRIMITIVE_TYPE_RECT, bool is_filled = false);

	// Functions to Draw, called by main
	void Update();
	void Draw(SDL_Renderer* render_target);

	void Draw_Spritesheet_Directly(SDL_Renderer* render_target, int spritesheet_num, SDL_Rect position_rect, SDL_Rect clip_rect, bool is_multisprite = false, int multisprite_num = 0, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip render_flip = SDL_FLIP_NONE);
	void Draw_Primitive_Directly(SDL_Renderer* render_target, SDL_Rect primitive_rect, SDL_Color primitive_color, bool filled);
	void Draw_Spritesheet_Instructions(SDL_Renderer* render_target, int spritehsheet);
	void Draw_Primitive_Instructions(SDL_Renderer* render_target, int layer = 1);
	void Draw_Text_Strings(SDL_Renderer* render_target, int layer = 1);

	// Free
	void free();

private:
	Global_Service_Locator * service_locator;

	int count_num_fonts = 0;

	int count = 0;

	bool reset_prebaked_spritesheets = false;

	// Window Format
	Uint32 window_format;

	// Font Array
	FC_Font* font_array[MAX_NUM_FONTS];

	//Spritesheets
	Spritesheet background_spritesheet;
	Spritesheet base_spritesheet; // simple tile_based sprites 
	Spritesheet base_overlay; // overlay of simple tile based sprites 
	Spritesheet mid_spritesheet;
	Spritesheet mid_overlay_spritesheet;
	Spritesheet mid_2_spritesheet;
	Spritesheet entity_spritesheet;
	Spritesheet icon_spritesheet;
	Spritesheet projectile_spritesheet;

	// Multisprites // composite sprites built up of smaller sprites
	int current_num_base_multisprites;
	Spritesheet base_multisprite[MAX_SPRITES_PER_MULTISPRITE];
	int current_num_mid_multisprites;
	Spritesheet mid_multisprite[MAX_SPRITES_PER_MULTISPRITE];

	// Pre_Baked Larger Multisprites
	Spritesheet mid_prebaked_large_multisprite;

	// Primitive arrays 
	int count_num_primitives = -1;
	Primitive_Instruction Primitive_Instruction_Array[MAX_NUM_PRIMITIVES];

	// Text print array
	int count_num_print_text = -1;
	Text_Instruction Text_Instruction_Array[MAX_NUM_TEXT_PRINT];

	int count_num_baked_layers = 6;
	Draw_Layer* baked_draw_layer_array[6];
};

