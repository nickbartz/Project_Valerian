#pragma once

#include<Global_Constants.h>
#include<Draw_System.h>
#include<SDL.h>

class Cursor;

class Console_Component
{
public:
	Console_Component(SDL_Rect placement_rect = { 5,5,100,100 }, bool component_is_filled = false, SDL_Color bColor = { 50,50,255,100 }, bool is_highlight = true, SDL_Color hColor = { 255,255,255,255 });

	// Functions that determine what sort component this becomes
	void Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor);
	void Assign_Window(int window_name);
	void Assign_Panel(int panel_name);
	void Set_Font_Type(int font_type);

	// Accessors
	SDL_Rect Return_Rect();
	void Change_Offset_Rect(int argument, int new_value);
	int Get_Font_Type();
	string Get_Title();
	int Get_Panel();
	int Get_Window();

	// Functions that check for clicks
	bool Currently_Clicked(); // return current state
	bool Currently_Clicked(bool state_override); // return current state and also set current state
	void Check_For_Click(Cursor* cursor);

	// Base draw_functions for the component
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);

protected:

	// Variables relating to the component if it is a simple button
	bool draw_title = false;
	string title= "";
	SDL_Rect text_offset;
	int font_type = FONT_DEFAULT;
	SDL_Color text_color;

	// Variables related to the function of the component if it is a button
	int window = WINDOW_NULL;
	int panel = PANEL_NULL;
	int button_action = BUTTON_ACTION_DO_NOTHING;
	bool currently_clicked = false;

	// Variables relating to the placementof the underlying rect
	SDL_Rect offset_rect;
	SDL_Rect current_screen_rect;

	// Variables relating to the colors of the underlying rect
	bool filled;
	SDL_Color fill_color;
	bool highlight;
	SDL_Color highlight_color;

};

class Message_Stream : public Console_Component
{
public:
	Message_Stream(int window_name = WINDOW_NULL, int panel_name = PANEL_NULL, int button_action = BUTTON_ACTION_DO_NOTHING, SDL_Rect placement_rect = { 5,5,100,100 }, int max_lines = 20) :Console_Component(placement_rect)
	{
		max_message_lines = max_lines;

		for (int i = 0; i < UI_MAX_CONSOLE_MESSAGES; i++)
		{
			message_array[i] = "-";
		}
	}

	void Push_Message_Into_Stream(string new_message);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);

private:
	int max_message_lines;

	string message_array[UI_MAX_CONSOLE_MESSAGES];

};


