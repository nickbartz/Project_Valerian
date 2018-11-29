#pragma once

using namespace std;
#include<string>
#include<Global_Constants.h>
#include<SDL.h>
#include<map>
#include<iostream>


class SDL_Texture;
class Global_Service_Locator;

class Draw_Layer
{
public:
	Draw_Layer(Global_Service_Locator* service_locator = NULL);

	void Send_Draw_Instruction(int render_object_id, bool is_primitive, int spritesheet_id, SDL_Rect src_rect, SDL_Rect dest_rect, int primitive_type, bool is_filled, bool has_color, SDL_Color primitive_color, int multisprite_state = 0, int multisprite_num = 0);
	int Get_Current_Instruction_Num();

	void Init();
	void Draw();
	void free();


private:
	Global_Service_Locator * service_locator = NULL;

	SDL_Rect screen_rect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_Rect layer_rect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_Texture * layer_texture;
	SDL_Texture* clear_texture;
	SDL_Renderer* game_renderer;

	struct Draw_Instruction
	{
		int init = 0;
		bool is_primitive = false;
		int spritesheet_id = 0;
		SDL_Rect src_rect = { 0,0,0,0 };
		SDL_Rect dest_rect = { 0,0,0,0 };
		int primitive_type = 0;
		bool is_filled = false;
		bool has_color = false;
		SDL_Color primitive_color = { 0,0,0,0 };
		int multisprite_state = 0;
		int multisprite_num = 0;

		bool operator!=(const Draw_Instruction& new_instruction) const
		{
			if (new_instruction.is_primitive != is_primitive) return true;
			else if (new_instruction.is_primitive == true)
			{
				if (new_instruction.primitive_type != primitive_type) return true;
				if (new_instruction.is_filled != is_filled) return true;
				if (new_instruction.has_color != has_color) return true;
				else if (new_instruction.has_color == true)
				{
					if (new_instruction.primitive_color.r != primitive_color.r) return true;
					if (new_instruction.primitive_color.g != primitive_color.g) return true;
					if (new_instruction.primitive_color.b != primitive_color.b) return true;
					if (new_instruction.primitive_color.a != primitive_color.a) return true;
				}
			}
			if (new_instruction.spritesheet_id != spritesheet_id) return true;
			if (new_instruction.src_rect.x != src_rect.x) return true;
			if (new_instruction.src_rect.y != src_rect.y) return true;
			if (new_instruction.src_rect.w != src_rect.w) return true;
			if (new_instruction.src_rect.h != src_rect.h) return true;
			if (new_instruction.dest_rect.x != dest_rect.x) return true;
			if (new_instruction.dest_rect.y != dest_rect.y) return true;
			if (new_instruction.dest_rect.w != dest_rect.w) return true;
			if (new_instruction.dest_rect.h != dest_rect.h) return true;

			return false;
		}
	};

	int current_num_instructions = 0;
	Draw_Instruction pre_baked_instructions[MAX_SPRITE_INSTRUCTIONS];

	void Set_Layer_As_Render_Target(bool set_to_target);

	void Stamp_Draw_Instruction(Draw_Instruction new_instruction);
	void Stamp_From_Spritesheet(int spritesheet, SDL_Rect src_rect, SDL_Rect dest_rect, int multisprite_state = 0, int multisprite_num = 0);
	void Stamp_Primitive(int primitive_type, SDL_Rect primitive_rect, SDL_Color primitive_color, bool is_filled);
	void Clear_Texture_Space(SDL_Rect clear_size);

};