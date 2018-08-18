#include <SDL.h>
#include <SDL_image.h>
#include <SDL_FontCache.h>
#include <stdio.h>
#include<Global_Constants.h>
#include <Message_Bus.h>

// DISPLAY VARIABLES
SDL_Renderer* Game_Renderer = NULL;
SDL_Window* Game_Window = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		Game_Window = SDL_CreateWindow("Project Valerian", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
		if (Game_Window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			Game_Renderer = SDL_CreateRenderer(Game_Window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) | SDL_RENDERER_TARGETTEXTURE);
			if (Game_Renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(Game_Renderer, 0x0, 0x0, 0x0, 0x0);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				// We will disable the cursor later once we returna new icon for the mouse, but for now leave it on
				//SDL_ShowCursor(SDL_DISABLE);
			}
		}
	}

	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(Game_Renderer);
	SDL_DestroyWindow(Game_Window);
	Game_Window = NULL;
	Game_Renderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

int main(int argc, char *args[])
{
	SDL_Event e;

	bool quit = false;

	// Run startup functions
	init();

	// Create Message Bus
	Message_Bus Main_Bus;

	while (!quit)
	{

		while (SDL_PollEvent(&e) != 0)
		{			
			//User requests quit
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}

			Main_Bus.Add_Input_Message(e);
		}

		//Clear screen
		SDL_SetRenderDrawColor(Game_Renderer, 0x0, 0x0, 0x0, 0x0);
		SDL_RenderClear(Game_Renderer);

		//Update screen
		SDL_RenderPresent(Game_Renderer);

		// Clear the message bus
		Main_Bus.Clear_Input_Messages();
	}

	close();

	return 0;
}