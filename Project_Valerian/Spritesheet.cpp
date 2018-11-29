#include <Spritesheet.h>
#include <iostream>
#include <stdio.h>

Spritesheet::Spritesheet()
{
	uniq_id = rand() % 1000;
}

void Spritesheet::Init(std::string path, SDL_Renderer* game_renderer, Uint32 window_format)
{
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		//SDL_PixelFormat* mappingFormat = SDL_AllocFormat(window_format);
		//SDL_ConvertSurface(loadedSurface, mappingFormat, NULL);

		newTexture = SDL_CreateTextureFromSurface(game_renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			spritesheet_w = loadedSurface->w;
			spritesheet_h = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	spritesheet_texture = newTexture;
	init = true;
}

void Spritesheet::Init_As_Multisprite(SDL_Renderer* game_renderer, int sprite_width, int sprite_height)
{
	if (spritesheet_texture != NULL) SDL_DestroyTexture(spritesheet_texture);
	spritesheet_texture = SDL_CreateTexture(game_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sprite_width, sprite_height);
	init = true;
}

void Spritesheet::Initialize_Prebaked_Texture_Layer(SDL_Renderer* game_renderer, int width, int height)
{
	if (prebaked_layer_texture != NULL) SDL_DestroyTexture(prebaked_layer_texture);
	prebaked_layer_texture = SDL_CreateTexture(game_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
}

void Spritesheet::Deinitialize()
{
	init = false;
}

bool Spritesheet::is_init()
{
	return init;
}

void Spritesheet::Draw_Directly(SDL_Renderer* game_renderer, SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip)
{
	// Reset Global Color, Blend, and Alpha Variables 
	SDL_SetTextureBlendMode(spritesheet_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(spritesheet_texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(spritesheet_texture, 255);

	// Stamp to texture
	SDL_RenderCopyEx(game_renderer, spritesheet_texture, &clip_rect, &position_rect, angle, center, render_flip);
}

void Spritesheet::Draw(SDL_Renderer* game_renderer, bool use_prebaked)
{
	if (use_prebaked == false)
	{
		for (int i = 0; i < current_num_instructions; i++)
		{
			SDL_SetTextureBlendMode(spritesheet_texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(spritesheet_texture, instruction_array[i].color_shift.r, instruction_array[i].color_shift.g, instruction_array[i].color_shift.b);
			SDL_SetTextureAlphaMod(spritesheet_texture, instruction_array[i].color_shift.a);
			SDL_RenderCopyEx(game_renderer, spritesheet_texture, &instruction_array[i].clip_rect, &instruction_array[i].position_rect, instruction_array[i].angle, instruction_array[i].center, instruction_array[i].flip);
		}

		// After the spritesheet draws it also automatically flushes its instuctions array so no need for a dedicated clear instructions function
		current_num_instructions = 0;
	}
	else if (use_prebaked == true)
	{
		if (prebaked_layer_init == false) Draw_Sprite_Instructions_To_Texture(game_renderer);
		else if (prebaked_layer_init == true)
		{
			Draw_Prebaked_Texture(game_renderer);
		}

		current_num_instructions = 0;
	}

}

bool Spritesheet::Get_Prebake_Init()
{
	return prebaked_layer_init;
}

void Spritesheet::Set_Prebake_Init(bool init)
{
	prebaked_layer_init = init;
}

void Spritesheet::Draw_Sprite_Instructions_To_Texture(SDL_Renderer* game_renderer)
{
	SDL_SetRenderTarget(game_renderer, prebaked_layer_texture);
	SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 0);
	SDL_RenderClear(game_renderer);

	for (int i = 0; i < current_num_instructions; i++)
	{
		SDL_RenderCopyEx(game_renderer, spritesheet_texture, &instruction_array[i].clip_rect, &instruction_array[i].position_rect, instruction_array[i].angle, instruction_array[i].center, instruction_array[i].flip);
	}

	SDL_SetRenderTarget(game_renderer, NULL);

	prebaked_layer_init = true;
}

void Spritesheet::Draw_Prebaked_Texture(SDL_Renderer* game_renderer)
{
	SDL_Rect screen_rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	SDL_Rect texture_clip = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	
	// Reset Global Color, Blend, and Alpha Variables 
	SDL_SetTextureBlendMode(spritesheet_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(spritesheet_texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(spritesheet_texture, 255);

	// Draw pre-baked texture
	SDL_RenderCopyEx(game_renderer, prebaked_layer_texture, &texture_clip, &screen_rect, 0.0, NULL, SDL_FLIP_NONE);

}

void Spritesheet::Add_Sprite_Instructions(SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip, SDL_Color color_shift)
{
	Sprite_Instructions new_instruction = { position_rect, clip_rect,angle,center,render_flip, color_shift};
	if (current_num_instructions < MAX_SPRITE_INSTRUCTIONS)
	{
		//cout << current_num_instructions << endl;
		instruction_array[current_num_instructions] = new_instruction;
		current_num_instructions++;
	}
	else
	{
		current_num_instructions = 0;
		std::cout << "warning, sprite instruction overflow" << std::endl;
	}
}

void Spritesheet::Set_SDL_Texture_As_Render_Target(SDL_Renderer* game_renderer)
{
	if (spritesheet_texture != NULL)
	{
		if (SDL_SetRenderTarget(game_renderer, spritesheet_texture) != 0)
		{
			cout << "error setting texture as render target" << endl;
		}
	}
	else
	{
		cout << "cannot set null texture as render target" << endl;
	}

}

void Spritesheet::Sprite_Clear_Instructions()
{
	current_num_instructions = 0;
}

void Spritesheet::free()
{
	if (spritesheet_texture != NULL)
	{
		SDL_DestroyTexture(spritesheet_texture);
		spritesheet_texture = NULL;
		spritesheet_w = 0;
		spritesheet_h = 0;
	}
}

int Spritesheet::return_uniq_id()
{
	return uniq_id;
}