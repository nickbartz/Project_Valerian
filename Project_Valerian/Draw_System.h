#pragma once

#include<Global_Constants.h>
#include<SDL.h>

class Draw_System
{
public:
	Draw_System();

	// The standard primitive rectangle that will be used to instruct the renderer
	struct Primitive_Instruction
	{
		int init = 0;
		SDL_Rect pos_rect = { 0,0,0,0 };
		bool filled = false;
		SDL_Color primitive_color = { 255,255,255,255 };
	};

	// Functions to Add or Remove From the Draw Array
	void Add_Primitive_To_Render_Cycle(Primitive_Instruction primitive);
	void Clear_Primitive_Instruction_Array();

	// Function to Draw, called by main
	void Draw_Primitives(SDL_Renderer* render_target);

	// Free
	void free();

private:

	// Primitive array 
	int count_num_primitives = -1;
	Primitive_Instruction Primitive_Instruction_Array[MAX_NUM_PRIMITIVES];

};