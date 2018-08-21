#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include<Global_Constants.h>

#include<UI.h>
#include<Cursor.h>
#include<Message_Bus.h>
#include<Draw_System.h>
#include<Service_Locator.h>

#include<Timer.h>

using namespace std;

// DISPLAY VARIABLES
SDL_Renderer* Game_Renderer = NULL;
SDL_Window* Game_Window = NULL;

FC_Font* font_array[MAX_NUM_FONTS];

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
			Game_Renderer = SDL_CreateRenderer(Game_Window, -1, (SDL_RENDERER_ACCELERATED ) | SDL_RENDERER_TARGETTEXTURE);
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

void load_assets(SDL_Renderer* game_renderer)
{
	font_array[FONT_DEFAULT] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_DEFAULT], game_renderer, "Fonts/OpenSans-Regular.ttf", 12, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_LARGE] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_LARGE], game_renderer, "Fonts/OpenSans-Regular.ttf", 14, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_LARGE_BOLD] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_LARGE_BOLD], game_renderer, "Fonts/OpenSans-Bold.ttf", 14, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_SMALL] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_SMALL], game_renderer, "Fonts/OpenSans-Regular.ttf", 10, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);
}

void free_assets()
{
	for (int i = 0; i < MAX_NUM_FONTS; i++)
	{
		FC_ClearFont(font_array[i]);
		delete font_array[i];
		font_array[i] = NULL;
	}
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
	
	// Create the Timer
	LTimer fpsTimer;

	bool quit = false;

	// Run startup functions
	init();
	load_assets(Game_Renderer);

	// Create Service Locator
	unique_ptr<Service_Locator> service_locator(new Service_Locator());

	// Create Draw System
	unique_ptr<Draw_System> draw_system(new Draw_System{Game_Renderer, font_array});

	// Create UI System
	unique_ptr<UI> user_interface(new UI(service_locator.get()));

	// Create Cursor
	unique_ptr<Cursor> cursor(new Cursor(service_locator.get()));

	// Create Message Bus
	unique_ptr<Message_Bus> main_bus(new Message_Bus(service_locator.get()));

	// Register pointers with the service locator
	service_locator->Register_Draw_System_Pointer(draw_system.get());
	service_locator->Register_MB_Pointer(main_bus.get());
	service_locator->Register_UI_Pointer(user_interface.get());
	service_locator->Register_Cursor_Pointer(cursor.get());

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	while (!quit)
	{	
		while (SDL_PollEvent(&e) != 0)
		{			
			//User requests quit
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}

			if ((e.type >= 768 && e.type <= 772) || (e.type >= 1024 && e.type <= 1027)) main_bus->Add_Input_Message(e);
		}

		// Push Messages From the Main Bus Out to All Subsystems
		main_bus->Push_Messages();

		// Update components - they will also send messages back into the Main Bus
		cursor->Update();
		user_interface->Update();

		//Clear screen
		SDL_SetRenderDrawColor(Game_Renderer, 0x0, 0x0, 0x0, 0x0);
		SDL_RenderClear(Game_Renderer);
		
		// Draw objects on screen
		draw_system->Draw_Primitives(Game_Renderer);
		draw_system->Draw_Text_Strings(Game_Renderer);

		// Draw FPS
		int avgFPS = countedFrames / (fpsTimer.getTicks() / 10000.f);
		if (avgFPS > 2000000) avgFPS = 0;
		string fps = std::to_string(avgFPS);
		FC_Draw(font_array[FONT_DEFAULT], Game_Renderer, 0, 0, fps.c_str());

		//Update screen
		SDL_RenderPresent(Game_Renderer);

		// Clear Draw Instructions
		draw_system->Clear_Primitive_Instruction_Array();
		draw_system->Clear_Text_Instruction_Array();

		// Clear the message bus
		main_bus->Clear_Input_Messages();

		++countedFrames;
	}

	// Free and Remove objects created on the heap
	main_bus->free();
	draw_system->free();
	fpsTimer.free();

	// Unload Assets
	free_assets();

	// Unload all major systems
	close();

	cout << "Successfully Closed Application" << endl;

	return 0;
}