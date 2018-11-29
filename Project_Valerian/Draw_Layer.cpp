#include<Draw_Layer.h>
#include<Service_Locator.h>
#include<iostream>
#include<Draw_System.h>
#include<Object.h>


Draw_Layer::Draw_Layer(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Draw_Layer::Init()
{
	game_renderer = service_locator->get_Game_Renderer();
	SDL_SetRenderDrawBlendMode(game_renderer, SDL_BLENDMODE_BLEND);
	layer_texture = SDL_CreateTexture(game_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	clear_texture = SDL_CreateTexture(game_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetTextureBlendMode(layer_texture, SDL_BLENDMODE_BLEND);
}

int Draw_Layer::Get_Current_Instruction_Num()
{
	return current_num_instructions;
}

void Draw_Layer::Set_Layer_As_Render_Target(bool set_to_target)
{
	if (set_to_target)
	{
		if (SDL_SetRenderTarget(game_renderer, layer_texture) != 0)
		{
			cout << "error setting texture as render target" << endl;
		}
	}
	else
	{
		if (SDL_SetRenderTarget(game_renderer, NULL) != 0)
		{
			cout << "error setting target back to screen" << endl;
		}
	}

}

void Draw_Layer::Stamp_Draw_Instruction(Draw_Instruction new_instruction)
{
	if (new_instruction.is_primitive)
	{
		Stamp_Primitive(new_instruction.primitive_type, new_instruction.dest_rect, new_instruction.primitive_color, new_instruction.is_filled);
	}
	else
	{
		Stamp_From_Spritesheet(new_instruction.spritesheet_id, new_instruction.src_rect, new_instruction.dest_rect, new_instruction.is_multisprite, new_instruction.multisprite_num);
	}
}

void Draw_Layer::Stamp_From_Spritesheet(int spritesheet, SDL_Rect src_rect, SDL_Rect dest_rect, bool is_multisprite, int multisprite_num)
{
	Set_Layer_As_Render_Target(true);
	service_locator->get_Draw_System_Pointer()->Draw_Spritesheet_Directly(game_renderer, spritesheet, dest_rect, src_rect, is_multisprite, multisprite_num);
	Set_Layer_As_Render_Target(false);
}

void Draw_Layer::Stamp_Primitive(int primitive_type, SDL_Rect primitive_rect, SDL_Color primitive_color, bool is_filled)
{
	Set_Layer_As_Render_Target(true);
	service_locator->get_Draw_System_Pointer()->Draw_Primitive_Directly(game_renderer, primitive_rect, primitive_color, is_filled);
	Set_Layer_As_Render_Target(false);
}

void Draw_Layer::Clear_Texture_Space(SDL_Rect clear_size)
{
	Set_Layer_As_Render_Target(true);
	SDL_RenderCopyEx(game_renderer, clear_texture, &clear_size, &clear_size, 0.0, NULL, SDL_FLIP_NONE);
	Set_Layer_As_Render_Target(false);
}

void Draw_Layer::Draw()
{
	for (int i = 0; i < current_num_instructions + 10; i++)
	{
		// Check to see if the current berth is even in use
		if (pre_baked_instructions[i].init != DRAW_LAYER_STATE_UNUSED)
		{
			// If the draw instruction has not been re-pushed this cycle, clear it from the baked texture
			if (pre_baked_instructions[i].init == DRAW_LAYER_STATE_ATROPHIED)
			{
				Clear_Texture_Space(pre_baked_instructions[i].dest_rect);
				pre_baked_instructions[i].init = DRAW_LAYER_STATE_UNUSED;
			}
			else 
			{
				if (pre_baked_instructions[i].init == DRAW_LAYER_STATE_DRAW_AGAIN)
				{
					Stamp_Draw_Instruction(pre_baked_instructions[i]);
				}

				pre_baked_instructions[i].init = DRAW_LAYER_STATE_ATROPHIED;
			}
		}
	}

	//Actually draw the texture;
	SDL_RenderCopyEx(game_renderer,layer_texture,&layer_rect, &screen_rect, 0.0, NULL, SDL_FLIP_NONE);

	// Reset the ticker
	current_num_instructions = 0;
}

void Draw_Layer::Send_Draw_Instruction(int render_object_id, bool is_primitive, int spritesheet_id, SDL_Rect src_rect, SDL_Rect dest_rect, int primitive_type, bool is_filled, bool has_color, SDL_Color primitive_color, bool is_multisprite, int multisprite_num)
{
	Draw_Instruction new_instruction = { 1, is_primitive, spritesheet_id, src_rect, dest_rect, primitive_type, is_filled, has_color, primitive_color, is_multisprite, multisprite_num };

	if (pre_baked_instructions[render_object_id].init != DRAW_LAYER_STATE_UNUSED )
	{
		if (new_instruction != pre_baked_instructions[render_object_id])
		{
			Clear_Texture_Space(pre_baked_instructions[render_object_id].dest_rect);
			new_instruction.init = DRAW_LAYER_STATE_DRAW_AGAIN;
			pre_baked_instructions[render_object_id] = new_instruction;
		}
		else
		{
			pre_baked_instructions[render_object_id].init = DRAW_LAYER_STATE_DONT_DRAW;
		}
	}
	else
	{
		new_instruction.init = DRAW_LAYER_STATE_DRAW_AGAIN;
		pre_baked_instructions[render_object_id] = new_instruction;
	}

	current_num_instructions++;
}