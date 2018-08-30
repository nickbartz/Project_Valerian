#include<Draw_System.h>

Draw_System::Draw_System(SDL_Renderer* game_renderer, FC_Font* font_array_start[])
{
	count_num_primitives = 0;
	count_num_print_text = 0;

	// MUST REMEMBER TO ADD THIS WHEN DELETING FONTS
	for (int p = 0; p < MAX_NUM_FONTS; ++p) { font_array[p] = font_array_start[p]; };
}

void Draw_System::Init_Sprites(SDL_Renderer* game_renderer)
{
	background_spritesheet.Init("Sprites/background_spritesheet.png", game_renderer);
	base_spritesheet.Init("Sprites/base_tile_spritesheet.png", game_renderer);
	mid_spritesheet.Init("Sprites/mid_tile_spritesheet.png", game_renderer);
}

// Query Functions

int Draw_System::Return_Text_Width(int font_type, string text)
{
	return FC_GetWidth(font_array[font_type], text.c_str());
}


// Functions for adding or removing draw jobs from render cycle
void Draw_System::Add_Sprite_Render_Job_To_Render_Cycle(int spritesheet, SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip)
{
	switch (spritesheet)
	{
	case SPRITESHEET_BACKGROUND:
		background_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_BASE:
		base_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	}
}

void Draw_System::Add_Primitive_To_Render_Cycle(Primitive_Instruction primitive)
{
	Primitive_Instruction_Array[count_num_primitives] = primitive;

	count_num_primitives++;
	if (count_num_primitives > MAX_NUM_PRIMITIVES) count_num_primitives = 0;
}

void Draw_System::Clear_Primitive_Instruction_Array()
{
	for (int i = 0; i < MAX_NUM_PRIMITIVES; i++)
	{
		Primitive_Instruction_Array[i].init = 0;
	}

	count_num_primitives = 0;
}

void Draw_System::Add_Text_Job_To_Render_Cycle(Text_Instruction string)
{
	Text_Instruction_Array[count_num_print_text] = string;
	count_num_print_text++;
	if (count_num_print_text > MAX_NUM_TEXT_PRINT) count_num_print_text = 0;
}

void Draw_System::Clear_Text_Instruction_Array()
{
	for (int i = 0; i < MAX_NUM_TEXT_PRINT; i++)
	{
		Text_Instruction_Array[i].init = 0;
	}

	count_num_print_text = 0;
}


// Full Draw Function

void Draw_System::Draw(SDL_Renderer* render_target)
{
	Draw_Sprites(render_target);
	Draw_Primitives(render_target);
	Draw_Text_Strings(render_target);
}

// Functions for drawing primitives

void Draw_System::Draw_Sprites(SDL_Renderer* render_target)
{
	background_spritesheet.Draw(render_target);
	base_spritesheet.Draw(render_target);
}

void Draw_System::Draw_Primitives(SDL_Renderer* render_target)
{
	for (int i = 0; i < count_num_primitives; i++)
	{
		if (Primitive_Instruction_Array[i].init == 1)
		{
			SDL_Rect rect_pos = Primitive_Instruction_Array[i].pos_rect;
			SDL_Color rect_color = Primitive_Instruction_Array[i].primitive_color;
			SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
			SDL_SetRenderDrawBlendMode(render_target, SDL_BLENDMODE_BLEND);
			if (Primitive_Instruction_Array[i].filled == true)
			{
				SDL_RenderFillRect(render_target, &rect_pos);
			}
			else SDL_RenderDrawRect(render_target, &rect_pos);
		}
	}
}

void Draw_System::Draw_Text_Strings(SDL_Renderer* render_target)
{
	for (int i = 0; i < count_num_print_text + 1; i++)
	{
		if (Text_Instruction_Array[i].init == 1)
		{			
			SDL_Rect rect_pos = Text_Instruction_Array[i].pos_rect;
			int font_num = Text_Instruction_Array[i].font;
			const char* text_array = Text_Instruction_Array[i].text_string.c_str();

			FC_Draw(font_array[font_num], render_target, rect_pos.x, rect_pos.y, text_array);
		}
	}

}

// 

void Draw_System::free()
{
	// Clear Fonts
	for (int i = 0; i < count_num_fonts; i++)
	{
		FC_ClearFont(font_array[i]);
		font_array[i] = NULL;
	}

	base_spritesheet.free();
}