#include<Draw_System.h>
#include<algorithm>
#include<iostream>

Draw_System::Draw_System(Global_Service_Locator* sLocator, FC_Font* font_array_start[], Uint32 wFormat)
{
	count_num_primitives = 0;
	count_num_print_text = 0;

	// MUST REMEMBER TO ADD THIS WHEN DELETING FONTS
	for (int p = 0; p < MAX_NUM_FONTS; ++p) { font_array[p] = font_array_start[p]; };

	service_locator = sLocator;

	window_format = wFormat;

	for (int i = 0; i < count_num_layers; i++)
	{
		draw_layer_array[i] = new Draw_Layer(service_locator);
		draw_layer_array[i]->Init();
	}
}

void Draw_System::Init_Sprites(SDL_Renderer* game_renderer)
{
	background_spritesheet.Init("Sprites/background_spritesheet.png", game_renderer, window_format );
	background_spritesheet.Initialize_Prebaked_Texture_Layer(game_renderer, SCREEN_WIDTH, SCREEN_HEIGHT); // This is option A for how to do prebaked textures 
	base_spritesheet.Init("Sprites/base_tile_spritesheet.png", game_renderer, window_format);
	mid_spritesheet.Init("Sprites/mid_tile_spritesheet.png", game_renderer, window_format);
	mid_2_spritesheet.Init("Sprites/mid_tile_spritesheet.png", game_renderer, window_format);
	entity_spritesheet.Init("Sprites/Entity_Spritesheet.png", game_renderer, window_format);
	icon_spritesheet.Init("Sprites/Icon_Spritesheet.png", game_renderer, window_format);
	projectile_spritesheet.Init("Sprites/Projectile_Spritesheet.png", game_renderer, window_format);

	mid_prebaked_large_multisprite.Init_As_Multisprite(game_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
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

bool Draw_System::Return_Spritesheet_Prebaked_Status(int spritesheet)
{
	switch (spritesheet)
	{
	case SPRITESHEET_BACKGROUND:
		return background_spritesheet.Get_Prebake_Init();
		break;
	}
}

void Draw_System::Set_Spritesheet_Prebaked_Status(int spritesheet_num, bool prebake_status)
{
	switch (spritesheet_num)
	{
	case SPRITESHEET_BACKGROUND:
		background_spritesheet.Set_Prebake_Init(prebake_status);
		break;
	}
}

void Draw_System::Set_Reset_Prebake_Status_Indicator(bool prebake_status)
{
	reset_prebaked_spritesheets = prebake_status;
}

SDL_Rect Draw_System::Get_Offset_Rect_For_Text(SDL_Rect offset_rect, int FONT_TYPE, string text, int vertical_alignment)
{
	int text_width = Return_Text_Width(FONT_TYPE, text);
	int text_height = Return_Text_Height(FONT_TYPE, text);

	SDL_Rect text_offset;

	text_offset.x = (offset_rect.w - text_width) / 2;
	text_offset.y = (offset_rect.h - text_height)/ 2;

	switch (vertical_alignment)
	{
	case 0:
		text_offset.x = 5;
		break;
	case 1:
		break;
	case 2:
		text_offset.x = offset_rect.w - text_width - 5;
		break;
	}

	return text_offset;
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
	if (spritesheet_num == SPRITESHEET_BASE) 	base_multisprite[multisprite_num].Set_SDL_Texture_As_Render_Target(service_locator->get_Game_Renderer());
	else if (spritesheet_num == SPRITESHEET_MID_1) mid_multisprite[multisprite_num].Set_SDL_Texture_As_Render_Target(service_locator->get_Game_Renderer());
	
	if (!clear_sprite)
	{
		// Draw diretly to that SDL_Texture using another spritesheet
		Draw_Spritesheet_Directly(service_locator->get_Game_Renderer(), spritesheet_num, stamp_rect, clip);
	}
	else
	{
		SDL_SetRenderDrawColor(service_locator->get_Game_Renderer(), 0, 0, 0, 0);
		SDL_RenderClear(service_locator->get_Game_Renderer());
		SDL_SetRenderDrawColor(service_locator->get_Game_Renderer(), 255, 255, 255, 255);
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
void Draw_System::Add_Sprite_Render_Job_To_Render_Cycle(int spritesheet, SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip, SDL_Color color_shift)
{
	switch (spritesheet)
	{
	case SPRITESHEET_BACKGROUND:
		background_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_BASE:
		base_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_BASE_OVERLAY:
		base_overlay.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_MID_1:
		mid_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_MID_1_OVERLAY:
		mid_overlay_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_MID_2:
		mid_2_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_ENTITY:
		entity_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_ICON:
		icon_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	case SPRITESHEET_PROJECTILE:
		projectile_spritesheet.Add_Sprite_Instructions(position_rect, clip_rect, angle, center, render_flip, color_shift);
		break;
	}
}

void Draw_System::Add_Multisprite_Render_Job_To_Render_Cycle(int spritesheet_num, int multi_tile_num, SDL_Rect pos_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip, SDL_Color color_shift)
{
	if (spritesheet_num == SPRITESHEET_BASE)
	{
		//base_multisprite[multi_tile_num].Add_Sprite_Instructions(pos_rect, { 0,0,TILE_SIZE,TILE_SIZE }, angle, center, render_flip, color_shift);

		int render_component_id = service_locator->get_Draw_System_Pointer()->Get_Layer_Uniq_Id(DRAW_LAYER_BASE);
		service_locator->get_Draw_System_Pointer()->Draw_Sprites_To_Draw_Layer(render_component_id, DRAW_LAYER_BASE, spritesheet_num, { 0,0,TILE_SIZE,TILE_SIZE }, pos_rect, true, multi_tile_num);
	}
	else if (spritesheet_num == SPRITESHEET_MID_1)
	{
		//mid_multisprite[multi_tile_num].Add_Sprite_Instructions(pos_rect, { 0,0,TILE_SIZE,TILE_SIZE }, angle, center, render_flip, color_shift);

		int render_component_id = service_locator->get_Draw_System_Pointer()->Get_Layer_Uniq_Id(DRAW_LAYER_MID);
		service_locator->get_Draw_System_Pointer()->Draw_Sprites_To_Draw_Layer(render_component_id, DRAW_LAYER_MID, spritesheet_num, { 0,0,TILE_SIZE,TILE_SIZE }, pos_rect, true, multi_tile_num);
	}
}

void Draw_System::Add_Primitive_To_Render_Cycle(int init, SDL_Rect pos_rect, bool filled, SDL_Color primitive_color, int primitive_type)
{
	Primitive_Instruction_Array[count_num_primitives] = Primitive_Instruction{ init, pos_rect, filled, primitive_color, primitive_type };

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

void Draw_System::Add_Text_Job_To_Render_Cycle(int init, SDL_Rect pos_rect, string text_string, int font, SDL_Color text_color)
{	
	Text_Instruction_Array[count_num_print_text] = Text_Instruction{ init, pos_rect, font, text_string, text_color };
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

void Draw_System::Update()
{
	if (reset_prebaked_spritesheets == true)
	{
		Set_Spritesheet_Prebaked_Status(SPRITESHEET_BACKGROUND, false);
		Set_Reset_Prebake_Status_Indicator(false);
	}
}

// Draw to layer functions
int Draw_System::Get_Layer_Uniq_Id(int render_layer)
{
	switch (render_layer)
	{
	case DRAW_LAYER_BACKGROUND:
		return draw_layer_array[DRAW_LAYER_BACKGROUND]->Get_Current_Instruction_Num();
		break;
	case DRAW_LAYER_BASE:
		return draw_layer_array[DRAW_LAYER_BASE]->Get_Current_Instruction_Num();
		break;
	case DRAW_LAYER_MID:
		return draw_layer_array[DRAW_LAYER_MID]->Get_Current_Instruction_Num();
		break;
	case DRAW_LAYER_MID_OVERLAY:
		return draw_layer_array[DRAW_LAYER_MID_OVERLAY]->Get_Current_Instruction_Num();
		break;
	}
}

void Draw_System::Draw_Sprites_To_Draw_Layer(int render_uniq_id, int draw_layer, int spritesheet, SDL_Rect src_rect, SDL_Rect dest_rect, bool is_multisprite, int multisprite_num , double angle, SDL_Point* center, SDL_RendererFlip flip, bool has_color, SDL_Color override_color, bool is_primitive, int primitive_id, bool is_filled)
{
	switch (draw_layer)
	{
	case DRAW_LAYER_BACKGROUND:
		draw_layer_array[DRAW_LAYER_BACKGROUND]->Send_Draw_Instruction(render_uniq_id, is_primitive, spritesheet, src_rect, dest_rect, primitive_id, is_filled, has_color, override_color, is_multisprite, multisprite_num);
		break;
	case DRAW_LAYER_BASE:
		draw_layer_array[DRAW_LAYER_BASE]->Send_Draw_Instruction(render_uniq_id, is_primitive, spritesheet, src_rect, dest_rect, primitive_id, is_filled, has_color, override_color, is_multisprite, multisprite_num);
		break;
	case DRAW_LAYER_MID:
		draw_layer_array[DRAW_LAYER_MID]->Send_Draw_Instruction(render_uniq_id, is_primitive, spritesheet, src_rect, dest_rect, primitive_id, is_filled, has_color, override_color, is_multisprite, multisprite_num);
		break;
	case DRAW_LAYER_MID_OVERLAY:
		draw_layer_array[DRAW_LAYER_MID_OVERLAY]->Send_Draw_Instruction(render_uniq_id, is_primitive, spritesheet, src_rect, dest_rect, primitive_id, is_filled, has_color, override_color, is_multisprite, multisprite_num);
		break;
	}
}

// Full Draw Function

void Draw_System::Draw(SDL_Renderer* render_target)
{
	draw_layer_array[DRAW_LAYER_BACKGROUND]->Draw();
	//Draw_Sprites(render_target, SPRITESHEET_BACKGROUND);
	draw_layer_array[DRAW_LAYER_BASE]->Draw();
	Draw_Sprites(render_target, SPRITESHEET_BASE);
	draw_layer_array[DRAW_LAYER_MID]->Draw();
	Draw_Sprites(render_target, SPRITESHEET_MID_1);
	Draw_Primitives_To_Screen(render_target, -1); // For lasers 
	Draw_Sprites(render_target, SPRITESHEET_ENTITY);
	Draw_Sprites(render_target, SPRITESHEET_PROJECTILE);
	Draw_Sprites(render_target, SPRITESHEET_MID_2);
	draw_layer_array[DRAW_LAYER_MID_OVERLAY]->Draw();
	Draw_Primitives_To_Screen(render_target, 1);

	Draw_Text_Strings(render_target);
	Draw_Sprites(render_target, SPRITESHEET_ICON);
	Draw_Primitives_To_Screen(render_target, 2);
	
	Draw_Text_Strings(render_target, 2);
}

// Functions for drawing primitives

void Draw_System::Draw_Spritesheet_Directly(SDL_Renderer* render_target, int spritesheet_num, SDL_Rect position_rect, SDL_Rect clip_rect, bool is_multisprite, int multisprite_num, double angle, SDL_Point* center, SDL_RendererFlip render_flip )
{
	switch (spritesheet_num)
	{
	case SPRITESHEET_BACKGROUND:
		background_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		break;
	case SPRITESHEET_BASE:
		if (!is_multisprite) base_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		else base_multisprite[multisprite_num].Draw_Directly(render_target, position_rect, clip_rect);
		break;
	case SPRITESHEET_BASE_OVERLAY:
		break;
	case SPRITESHEET_MID_1:
		if (!is_multisprite) mid_spritesheet.Draw_Directly(render_target, position_rect, clip_rect);
		else mid_multisprite[multisprite_num].Draw_Directly(render_target, position_rect, clip_rect);
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
		if (background_spritesheet.is_init() == true) background_spritesheet.Draw(render_target, true);
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

void Draw_System::Draw_Primitives_To_Screen(SDL_Renderer* render_target, int layer)
{
	//SDL_SetRenderDrawBlendMode(render_target, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < count_num_primitives; i++)
	{
		if (Primitive_Instruction_Array[i].init == layer)
		{
			SDL_Rect rect_pos = Primitive_Instruction_Array[i].pos_rect;
			SDL_Color rect_color = Primitive_Instruction_Array[i].primitive_color;

			switch (Primitive_Instruction_Array[i].primitive_type)
			{
			case PRIMITIVE_TYPE_RECT:
				SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
				if (Primitive_Instruction_Array[i].filled == true)
				{
					SDL_RenderFillRect(render_target, &rect_pos);
				}
				else SDL_RenderDrawRect(render_target, &rect_pos);
				break;
			case PRIMITIVE_TYPE_LINE:
				SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
				SDL_RenderDrawLine(render_target, rect_pos.x, rect_pos.y, rect_pos.w, rect_pos.h);
				break;
			case PRIMITIVE_TYPE_POINT:
				SDL_SetRenderDrawColor(render_target, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
				SDL_RenderDrawPoint(render_target, rect_pos.x, rect_pos.y);
				break;
			}
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
			
			string new_string = Text_Instruction_Array[i].text_string;

			const char* text_array = new_string.c_str();

			FC_Draw(font_array[font_num], render_target, rect_pos.x, rect_pos.y, text_array);
		}
	}

}

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