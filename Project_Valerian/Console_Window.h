#pragma once

#include<SDL.h>
#include<Console_Component.h>
#include<Console_Panel.h>

class Cursor;

class Console_Window
{
public:
	Console_Window(int window_name, string window_title, SDL_Rect console_window_rect);

	void Draw(Draw_System* draw_system);
	void Change_Rect(SDL_Rect new_rect);
	SDL_Rect Return_Rect();

protected:

	void Create_Panel_Header(SDL_Rect pos_rect, int panel_name, string panel_title, SDL_Rect text_offset, SDL_Color text_color);
	void Size_Panel_Headers();
	void Move_Window(Cursor* cursor);

	void Respond_To_Mouse(Cursor* cursor);
	
	Console_Component window_title_bar;
	int window_title_height = 25;
	int panel_bar_height = 25;
	int currently_active_panel = PANEL_NULL;
	int active_panel_headers = 0;
	Console_Component panel_headers[UI_MAX_WINDOW_HEADERS];
	SDL_Rect base_window_rect;

	int window_name;
};

class Player_Diagnostic_Window :public Console_Window
{
public:
	Player_Diagnostic_Window(SDL_Rect base_rect = { 100,100,300,500 }) :Console_Window(WINDOW_PLAYER_DIAGNOSTIC, "Config", base_rect)
	{
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_MESSAGE_STREAM, "Console", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_WORLD_INVENTORY, "World Inventory", { 5,3,0,0 }, { 255,255,255,255 });

		Size_Panel_Headers();

		message_panel = Console_Message_Panel(window_name,{ 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height + panel_bar_height });
		message_panel.active = true;

		currently_active_panel = PANEL_MESSAGE_STREAM;
	}

	void Respond_To_Mouse(Cursor* cursor);
	void Draw(Draw_System* draw_system);
	void Push_Message_Into_Stream(string message);

private:
	Console_Message_Panel message_panel;
};