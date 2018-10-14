#include<Draw_System.h>

Draw_System::Draw_System(Global_Service_Locator* sLocator, FC_Font* font_array_start[], Uint32 wFormat)
{
	count_num_primitives = 0;
	count_num_print_text = 0;

	// MUST REMEMBER TO ADD THIS WHEN DELETING FONTS
	for (int p = 0; p < MAX_NUM_FONTS; ++p) { font_array[p] = font_array_start[p]; };

	service_locator = sLocator;

	window_format = wFormat;
}

void Draw_System::Init_Sprites(SDL_Renderer* game_renderer)
{
	background_spritesheet.Init("Sprites/background_spritesheet.png", game_renderer, window_format );
	base_spritesheet.Init("Sprites/base_tile_spritesheet.png", game_renderer, window_format);
	mid_spritesheet.Init("Sprites/mid_tile_spritesheet.png", game_renderer, window_format);
	mid_2_spritesheet.Init("Sprites/mid_tile_spritesheet.png", game_renderer, window_format);
	entity_spritesheet.Init("Sprites/Entity_Spritesheet.png", game_renderer, window_format);
	icon_spritesheet.Init("Sprites/Icon_Spritesheet.png", game_renderer, window_format);
	projectile_spritesheet.Init("Sprites/Projectile_Spritesheet.png", game_renderer, window_format);
}

// Query Functions

int Draw_System::Return_Text_Width(int font_type, string text)
{
	return FC_GetWidth(font_array[font_type], text.c_str());
}

int Draw_System::Return_Text_Height(int font_type, string text)
{
	return FC_GetHeight(font_array[font_type], text.c_str());
}

// Multisprite Functions

int Draw_System::Add_New_Spritesheet_To_Multisprite(int spritesheet, SDL_Renderer* game_renderer)
{	
	if (spritesheet == SPRITESHEET_BASE)
	{
		current_num_base_multisprites++;
		for (int i = 0; i < current_num_base_multisprites; i++)
		{
			if (base_multisprite[i].is_init() == false)
			{
				base_multisprite[i].Init_As_Multisprite(game_renderer); 
				return i;
			}
		}
	}
	else if (spritesheet == SPRITESHEET_MID_1)
	{
		current_num_mid_multisprites++;
		for (int i = 0; i < current_num_mid_multisprites; i++)
		{
			if (mid_multisprite[i].is_init() == false)
			{
				mid_multisprite[i].Init_As_Multisprite(game_renderer);

				return i;
			}
		}
	}
}

void Draw_System::Stamp_Sprite_Onto_Multisprite(int spritesheet_num, int multisprite_num, SDL_Rect clip, SDL_Rect stamp_rect, bool clear_sprite)
{
	// Set the render target to the SDL_Texture on the multisprite of our choice
	if (spritesheet_num == SPRITESHEET_BASE) 	base_multisprite[multisprite_num].Set_Sprite_As_Render_Target(service_locator->get_Game_Renderer());
	else if (spritesheet_num == SPRITESHEET_MID_1) mid_multisprite[multisprite_num].Set_Sprite_As_Render_Target(service_locator->get_Game_Renderer());
	
	if (!clear_sprite)
	{
		// Draw diretly to that SDL_Texture using another spritesheet
		Draw_Spritesheet_Directly(service_locator->get_Game_Renderer(), spritesheet_num, stamp_rect, clip);
	}
	else
	{
		SDL_SetRenderDrawColor(service_locator->get_Game_Renderer(), 0, 0, 0, 0);
		SDL_RenderClear(service_locator->get_Game_Renderer());
	}

	// Reset our render target to our screen
	SDL_SetRenderTarget(service_locator->get_Game_Renderer(), NULL);
}

void Draw_System::Remove_Multisprite(int spritesheet_num, int multisprite_num)
{
	if (spritesheet_num == SPRITESHEET_BASE) base_multisprite[multisprite_num].Deinitialize();
	else if (spritesheet_num == SPRITESHEET_MID_1) mid_multisprite[multisprite_num].Deinitialize();
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
	case SPRITESHEET_BASE_OVERLAY:
		base_overlay.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_MID_1:
		mid_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_MID_1_OVERLAY:
		mid_overlay_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_MID_2:
		mid_2_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_ENTITY:
		entity_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_ICON:
		icon_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	case SPRITESHEET_PROJECTILE:
		projectile_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip);
		break;
	}
}

void Draw_System::Add_Multisprite_Render_Job_To_Render_Cycle(int spritesheet_num, int multi_tile_num, SDL_Rect pos_rect)
{
	if (spritesheet_num == SPRITESHEET_BASE) base_multisprite[multi_tile_num].Add_Sprite_Instructions(pos_rect, { 0,0,TILE_SIZE,TILE_SIZE });
	else if (spritesheet_num == SPRITESHEET_MID_1) mid_multisprite[multi_tile_num].Add_Sprite_Instructions(pos_rect, { 0,0,TILE_SIZE,TILE_SIZE });
}

void Draw_System::Add_Primitive_To_Render_Cycle(int init, SDL_Rect pos_rect, bool filled, SDL_Color primitive_color)
{
	Primitive_Instruction_Array[count_num_primitives] = Primitive_Instruction{ init, pos_rect, filled, primitive_color };

	count_num_primitives++;
	if (count_num_primitives > MAX_NUM_PRIMITIVES) count_num_primitives = 0;
}

void Draw_System::Clear_Primitive_Instruction_Array()
{
	for (int i = 0; i < count_num_primitives; i++)
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
	for (int i = 0; i < count_num_print_text; i++)
	{
		Text_Instruction_Array[i].init = 0;
	}

	count_num_print_text = 0;
}


// Full Draw Function

void Draw_System::Draw(SDL_Renderer* render_target)
{
	Draw_Sprites(render_target, SPRITESHEET_BACKGROUND);
	Draw_Sprites(render_target, SPRITESHEET_BASE);
	Draw_Sprites(render_target, SPRITESHEET_MID_1);
	Draw_Sprites(render_target, SPRITESHEET_ENTITY);
	Draw_Sprites(render_target, SPRITESHEET_MID_2);
	Draw_Sprites(render_target, SPRITESHEET_PROJECTILE);
	Draw_Primitives(render_target);
	Draw_Text_Strings(render_target);
	Draw_Sprites(render_target, SPRITESHEET_ICON);
	Draw_Primitives(render_target, 2);
	Draw_Text_Strings(render_target,2);

}

// Functions for drawing primitives

void Draw_System::Draw_Spritesheet_Directly(SDL_Renderer* render_target, int spritesheet_num, SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip)
{
	switch (spritesheet_num)
	{
	case SPRITESHEET_BACKGROUND:
		break;
	case SPRITESHEET_BASE:
		base_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		break;
	case SPRITESHEET_BASE_OVERLAY:
		break;
	case SPRITESHEET_MID_1:
		mid_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		break;
	case SPRITESHEET_MID_1_OVERLAY:
		break;
	case SPRITESHEET_MID_2:
		break;
	case SPRITESHEET_ENTITY:
		break;
	case SPRITESHEET_ROOF:
		break;
	case SPRITESHEET_ICON:
		icon_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		break;

	}
}

void Draw_System::Draw_Sprites(SDL_Renderer* render_target, int spritesheet)
{
	switch (spritesheet)
	{
	case SPRITESHEET_BACKGROUND:
		if (background_spritesheet.is_init() == true) background_spritesheet.Draw(render_target);
		break;
	case SPRITESHEET_BASE:
		if (base_spritesheet.is_init() == true) base_spritesheet.Draw(render_target);
		for (int i = 0; i < current_num_base_multisprites; i++) base_multisprite[i].Draw(render_target);
		break;
	case SPRITESHEET_BASE_OVERLAY:
		if (base_overlay.is_init() == true) base_overlay.Draw(render_target);
		break;
	case SPRITESHEET_MID_1:
		if (mid_spritesheet.is_init() == true) mid_spritesheet.Draw(render_target);
		for (int i = 0; i < current_num_mid_multisprites; i++) mid_multisprite[i].Draw(render_target);
		break;
	case SPRITESHEET_MID_1_OVERLAY:
		if (mid_overlay_spritesheet.is_init() == true) mid_overlay_spritesheet.Draw(render_target);
		break;
	case SPRITESHEET_MID_2:
		if (mid_2_spritesheet.is_init() == true) mid_2_spritesheet.Draw(render_target);
		break;
	case SPRITESHEET_ENTITY:
		if (entity_spritesheet.is_init() == true) entity_spritesheet.Draw(render_target);
		break;
	case SPRITESHEET_ICON:
		if (icon_spritesheet.is_init() == true) icon_spritesheet.Draw(render_target);
		break;
	case SPRITESHEET_PROJECTILE:
		if (projectile_spritesheet.is_init() == true) projectile_spritesheet.Draw(render_target);
		break;
	}
}

void Draw_System::Draw_Primitives(SDL_Renderer* render_target, int layer)
{
	SDL_SetRenderDrawBlendMode(render_target, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < count_num_primitives; i++)
	{
		if (Primitive_Instruction_Array[i].init == layer)
		{
			SDL_Rect rect_pos = Primitive_Instruction_Array[i].pos_rect;
			SDL_Color rect_color = Primitive_Instruction_Array[i].primitive_color;
			SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
			if (Primitive_Instruction_Array[i].filled == true)
			{
				SDL_RenderFillRect(render_target, &rect_pos);
			}
			else SDL_RenderDrawRect(render_target, &rect_pos);
		}
	}
}

void Draw_System::Draw_Primitive_Directly(SDL_Renderer* render_target, SDL_Rect primitive_rect, SDL_Color primitive_color, bool filled)
{
	SDL_SetRenderDrawBlendMode(render_target, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(render_target, primitive_color.r, primitive_color.g, primitive_color.b, primitive_color.a);

	if (filled == true)
	{
		SDL_RenderFillRect(render_target, &primitive_rect);
	}
	else SDL_RenderDrawRect(render_target, &primitive_rect);
}

void Draw_System::Draw_Text_Strings(SDL_Renderer* render_target, int layer)
{
	for (int i = 0; i < count_num_print_text + 1; i++)
	{
		if (Text_Instruction_Array[i].init == layer)
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

	background_spritesheet.free();
	base_spritesheet.free();
	for (int i = 0; i < current_num_base_multisprites; i++) base_multisprite[i].free();
	mid_spritesheet.free();
	for (int i = 0; i < current_num_mid_multisprites; i++) mid_multisprite[i].free();
	entity_spritesheet.free();
	mid_2_spritesheet.free();
	icon_spritesheet.free();
	projectile_spritesheet.free();
}