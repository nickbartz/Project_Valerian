#include<Draw_System.h>

Draw_System::Draw_System()
{
	count_num_primitives = -1;
}

// Functions for adding or removing primitives from render cycle
void Draw_System::Add_Primitive_To_Render_Cycle(Primitive_Instruction primitive)
{
	count_num_primitives++;
	if (count_num_primitives > MAX_NUM_PRIMITIVES) count_num_primitives = 0;

	Primitive_Instruction_Array[count_num_primitives] = primitive;
}

void Draw_System::Clear_Primitive_Instruction_Array()
{
	for (int i = 0; i < MAX_NUM_PRIMITIVES; i++)
	{
		Primitive_Instruction_Array[i].init = 0;
	}

	count_num_primitives = -1;
}

// Functions for drawing primitives
void Draw_System::Draw_Primitives(SDL_Renderer* render_target)
{
	for (int i = 0; i < MAX_NUM_PRIMITIVES; i++)
	{
		if (Primitive_Instruction_Array[i].init == 1)
		{
			SDL_Rect rect_pos = Primitive_Instruction_Array[i].pos_rect;
			SDL_Color rect_color = Primitive_Instruction_Array[i].primitive_color;
			SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
			if (Primitive_Instruction_Array[i].filled == true) SDL_RenderFillRect(render_target, &rect_pos);
			else SDL_RenderDrawRect(render_target, &rect_pos);
		}
	}
}

void Draw_System::free()
{

}