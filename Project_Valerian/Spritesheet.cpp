#include <Spritesheet.h>
#include <iostream>
#include <stdio.h>

Spritesheet::Spritesheet()
{

}

void Spritesheet::Init(std::string path, SDL_Renderer* game_renderer)
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
}

void Spritesheet::Draw(SDL_Renderer* game_renderer)
{
	for (int i = 0; i < current_num_instructions; i++)
	{
		SDL_RenderCopyEx(game_renderer, spritesheet_texture, &instruction_array[i].clip_rect, &instruction_array[i].position_rect, instruction_array[i].angle, NULL, instruction_array[i].flip);
	}
	
	current_num_instructions = 0;
}

void Spritesheet::Add_Sprite_Instructions(SDL_Rect position_rect, SDL_Rect clip_rect, double angle, SDL_Point center, SDL_RendererFlip render_flip)
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

