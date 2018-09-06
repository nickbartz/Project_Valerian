#pragma once

#include<SDL.h>

class UI;
class Message_Array;
class Draw_System;
class Cursor;
class Game_Library;


class Service_Locator
{
public:
	Service_Locator();

	UI * get_UI_pointer();
	Message_Array* get_MB_Pointer();
	Draw_System* get_Draw_System_Pointer();
	Cursor* get_Cursor_Pointer();
	Game_Library* get_Game_Library();
	SDL_Renderer* get_Game_Renderer();
	SDL_Window* get_Game_Window();

	void Register_UI_Pointer(UI* ui_pointer);
	void Register_MB_Pointer(Message_Array* msg_pointer);
	void Register_Draw_System_Pointer(Draw_System* draw_pointer);
	void Register_Cursor_Pointer(Cursor* cursor_pointer);
	void Register_Game_Library(Game_Library* game_library);
	void Register_Game_Renderer(SDL_Renderer* game_renderer);
	void Register_Game_Window(SDL_Window* gWindow);

	void free();

private:
	UI * UI_pointer;
	Message_Array* msg_bus_pointer;
	Draw_System* draw_system_pointer;
	Cursor* cursor_pointer;
	Game_Library* game_library;
	SDL_Renderer* game_renderer;
	SDL_Window* game_window;
};