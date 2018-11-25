#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include<Global_Constants.h>

#include<UI.h>
#include<Cursor.h>
#include<Draw_System.h>
#include<Service_Locator.h>
#include<Message_Array.h>
#include<Scene_Graph.h>
#include<Game_Library.h>
#include<Timer.h>
#include<A_Star.h>

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
		Game_Window = SDL_CreateWindow("Project Valerian", SCREEN_WIDTH/4, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
		if (Game_Window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			Game_Renderer = SDL_CreateRenderer(Game_Window, -1, (SDL_RENDERER_ACCELERATED) | SDL_RENDERER_TARGETTEXTURE);
			if (Game_Renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(Game_Renderer, 0x0, 0x0, 0x0, 0x0);
				SDL_SetRenderDrawBlendMode(Game_Renderer, SDL_BLENDMODE_BLEND);

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

				SDL_ShowCursor(SDL_DISABLE);
			}
		}
	}

	return success;
}

void load_assets(SDL_Renderer* game_renderer)
{
	font_array[FONT_DEFAULT] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_DEFAULT], game_renderer, "Fonts/OpenSans-Regular.ttf", 12, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_DEFAULT_BOLD] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_DEFAULT_BOLD], game_renderer, "Fonts/OpenSans-Bold.ttf", 12, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_LARGE] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_LARGE], game_renderer, "Fonts/OpenSans-Regular.ttf", 14, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_LARGE_BOLD] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_LARGE_BOLD], game_renderer, "Fonts/OpenSans-Bold.ttf", 14, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_SMALL] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_SMALL], game_renderer, "Fonts/OpenSans-Regular.ttf", 10, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	font_array[FONT_SMALL_BOLD] = FC_CreateFont();
	FC_LoadFont(font_array[FONT_SMALL_BOLD], game_renderer, "Fonts/OpenSans-Bold.ttf", 10, SDL_Color{ 255, 255, 255, 255 }, TTF_STYLE_NORMAL);
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
	unique_ptr<Global_Service_Locator> service_locator(new Global_Service_Locator());

	// Load Game Library
	unique_ptr<Game_Library> game_library(new Game_Library(service_locator.get()));

	// Create Draw System & Load Sprites
	unique_ptr<Draw_System> draw_system(new Draw_System{service_locator.get(), font_array, SDL_GetWindowPixelFormat(Game_Window) });
	draw_system.get()->Init_Sprites(Game_Renderer);

	// Create UI System
	unique_ptr<UI> user_interface(new UI(service_locator.get()));

	// Create Cursor
	unique_ptr<Cursor> cursor(new Cursor(service_locator.get()));

	// Create Message Bus
	unique_ptr<Message_Array> main_bus(new Message_Array(service_locator.get()));


	// Create World Map
	unique_ptr<Scene_Graph> scene_graph(new Scene_Graph(service_locator.get()));

	// Create the Pathfinder 
	unique_ptr<Path_Field> pathfinder(new Path_Field(service_locator.get()));

	// Register pointers with the service locator

	service_locator->Register_Game_Library(game_library.get());
	service_locator->Register_Game_Window(Game_Window);
	service_locator->Register_Game_Renderer(Game_Renderer);
	service_locator->Register_Draw_System_Pointer(draw_system.get());
	service_locator->Register_MB_Pointer(main_bus.get());
	service_locator->Register_UI_Pointer(user_interface.get());
	service_locator->Register_Cursor_Pointer(cursor.get());
	service_locator->Register_Scene_Graph(scene_graph.get());
	service_locator->Register_Pathfinder(pathfinder.get());

	user_interface.get()->Init();
	scene_graph->Create_Background();

	// TEST VARIABLES
	vector<vector<int>> new_room = game_library->Create_Room_From_Data_File(0, 0, "Data/test_room.csv");
	scene_graph->Stamp_Room_From_Array(new_room, -2, -2, 2);

	scene_graph->Create_New_Structure({ 8,-1 }, 4, 2);
	scene_graph->Create_New_Structure({ 9,-1 }, 14, 2);
	scene_graph->Create_New_Structure({ 10, -1 }, 15, 2);
	scene_graph->Create_New_Structure({ 12, -1 }, 24, 2);
	scene_graph->Return_Structure_At_Coord_By_Layer(9, -1, TILE_LAYER_MID)->Return_AI_Item_Component()->Add_Item_To_Inventory(1, 100, false, {});
	scene_graph->Return_Structure_At_Coord_By_Layer(9, -1, TILE_LAYER_MID)->Return_AI_Item_Component()->Add_Item_To_Inventory(2, 100, false, {});
	scene_graph->Return_Structure_At_Coord_By_Layer(9, -1, TILE_LAYER_MID)->Return_AI_Item_Component()->Add_Item_To_Inventory(3, 100, false, {});

	scene_graph->Create_New_Structure({ 6,-1 }, 39, 2);
	scene_graph->Return_Structure_At_Coord_By_Layer(6,-1, TILE_LAYER_MID)->Return_AI_Item_Component()->Add_Item_To_Inventory(7, 100, false, {});

	scene_graph->Create_New_Structure({ -5, -10 }, 21, 1);
	scene_graph->Create_New_Structure({ -5, -9 }, 22,1);
	scene_graph->Create_New_Structure({ -5, -8 }, 23, 1);
	
	scene_graph->Create_Entity({ 5,0 }, *game_library->Fetch_Entity_Template(2), 2);
	//scene_graph->Create_Entity({ 2,-1 }, *game_library->Fetch_Entity_Template(1), 2);

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	// END TEST VARIABLES
	int end_now = 0;
	bool pause = false;

	while (!quit)
	{	
		// Update subsystems 
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				quit = true;
			}

			// User requests pause
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
			{
				if (pause == true) pause = false;
				else if (pause == false) pause = true;
			}

			if ((e.type >= 768 && e.type <= 772) || (e.type >= 1024 && e.type <= 1027))
			{
				main_bus->Add_Message(Message_Input(e));
			}
		}

		draw_system->Update();
		user_interface->Update();
		scene_graph->Update();   // MAKES SURE ALL MESSAGES ARE SENT DURING THE UPDATE PROCESS
		cursor->Update();

		//Clear screen
		SDL_SetRenderDrawColor(Game_Renderer, 0x0, 0x0, 0x0, 0x0);
		SDL_RenderClear(Game_Renderer);
		
		// Send draw jobs from scene graph
		scene_graph->Draw();

		// Draw objects on screen
		draw_system->Draw(Game_Renderer);
		cursor->Draw();

		// Draw FPS
		int avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000) avgFPS = 0;
		string fps = std::to_string(avgFPS);
		FC_Draw(font_array[FONT_DEFAULT], Game_Renderer, 0, 0, fps.c_str());

		//Update screen
		SDL_RenderPresent(Game_Renderer);

		// Clear Draw Instructions
		draw_system->Clear_Primitive_Instruction_Array();
		draw_system->Clear_Text_Instruction_Array();

		// Have subsystems collect messages from the bus

		cursor->Collect_Bus_Messages();
		user_interface->Collect_Bus_Messages();
		scene_graph->Collect_Bus_Messages();

		// Clear the message bus
		main_bus->Clear_All();

		++countedFrames;

		//end_now += 1;
	}

	// Free and Remove objects created on the heap
	main_bus->free();
	draw_system->free();
	fpsTimer.free();

	// Unload Assets
	free_assets();

	// Unload all major systems
	close();

	std::cout << "Successfully Closed Application" << endl;

	return 0;
}