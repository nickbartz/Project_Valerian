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

void Spritesheet::Init_As_Multisprite(SDL_Renderer* game_renderer)
{
	if (spritesheet_texture != NULL) SDL_DestroyTexture(spritesheet_texture);
	spritesheet_texture = SDL_CreateTexture(game_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
	init = true;
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
	SDL_RenderCopyEx(game_renderer, spritesheet_texture, &clip_rect, &position_rect, angle, center, render_flip);
}

void Spritesheet::Draw(SDL_Renderer* game_renderer)
{
	for (int i = 0; i < current_num_instructions; i++)
	{
		SDL_RenderCopyEx(game_renderer, spritesheet_texture, &instruction_array[i].clip_rect, &instruction_array[i].position_rect, instruction_array[i].angle, instruction_array[i].center, instruction_array[i].flip);
		//SDL_RenderCopy(game_renderer, spritesheet_texture, &instruction_array[i].clip_rect, &instruction_array[i].position_rect);
	}
	
	// After the spritesheet draws it also automatically flushes its instuctions array so no need for a dedicated clear instructions function
	current_num_instructions = 0;
}

void Spritesheet::Add_Sprite_Instructions(SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point* center, SDL_RendererFlip render_flip)
{
	Sprite_Instructions new_instruction = { position_rect, clip_rect,angle,center,render_flip };
	if (current_num_instructions < MAX_SPRITE_INSTRUCTIONS)
	{
		instruction_array[current_num_instructions] = new_instruction;
		current_num_instructions++;
	}
	else
	{
		std::cout << "warning, sprite instruction overflow" << std::endl;
	}

}

void Spritesheet::Set_Sprite_As_Render_Target(SDL_Renderer* game_renderer)
{
	SDL_SetRenderTarget(game_renderer, spritesheet_texture);
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