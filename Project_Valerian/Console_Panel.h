#pragma once
#include<SDL.h>
#include<Console_Component.h>
#include<algorithm>

class Console_Panel
{
public:
	Console_Panel(int window_name, SDL_Rect rect = { 0,0,100,100 });
	int Get_Panel_Name();
	void Change_Rect(SDL_Rect new_rect);
	bool active = false;
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);

protected:
	int window;
	int panel_name;
	SDL_Rect offset_rect;
};

class Console_Message_Panel : public Console_Panel
{
public:
	Console_Message_Panel(int window_name = WINDOW_NULL, SDL_Rect offset_rect = { 0,0,100,100 }) : Console_Panel(window_name, offset_rect)
	{
		panel_name = PANEL_MESSAGE_STREAM;
		
		message_stream = Message_Stream(window, panel_name, BUTTON_ACTION_DO_NOTHING, { offset_rect.x,offset_rect.y,offset_rect.w,offset_rect.h }, min(UI_MAX_CONSOLE_MESSAGES, offset_rect.h / 20));
		message_stream.Set_Font_Type(FONT_SMALL);
	}
	
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Push_Message_Into_Stream(string message);

private:
	Message_Stream message_stream;

};