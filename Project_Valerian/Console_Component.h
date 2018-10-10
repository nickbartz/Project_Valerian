#pragma once

#include<Global_Constants.h>
#include<Draw_System.h>
#include<SDL.h>

class Cursor;
class Global_Service_Locator;

class UI_Component_Generic
{
public:
	UI_Component_Generic(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 5,5,100,100 }, bool component_is_filled = false, SDL_Color bColor = { 50,50,255,100 }, bool is_highlight = true, SDL_Color hColor = { 255,255,255,255 });

	void Init();

	// Functions that determine what sort component this becomes
	void Set_Click_Message(int message_length, int message_array[]);
	void Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor);
	void Assign_Window(int window_name);
	void Assign_Panel(int panel_name);
	void Set_Font_Type(int font_type);
	void Change_Component_Title(string new_title);

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
	void Check_For_Click();

	// Base draw_functions for the component
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);

protected:

	Global_Service_Locator * service_locator;

	// Variables relating to the component if it is a simple button
	bool draw_title = false;
	string title= "";
	SDL_Rect text_offset;
	int font_type = FONT_DEFAULT;
	SDL_Color text_color;

	// Variables related to the function of the component if it is a button // TO BE DEPRECATED
	int window = WINDOW_NULL;
	int panel = PANEL_NULL;
	int button_action = BUTTON_ACTION_DO_NOTHING;
	bool currently_clicked = false;

	// New Button Function
	int message_array[MAX_LENGTH_CUSTOM_MESSAGE];
	int message_length = 0;
	bool is_button = false;

	// Variables relating to the placementof the underlying rect
	SDL_Rect offset_rect;
	SDL_Rect current_screen_rect;

	// Variables relating to the colors of the underlying rect
	bool filled;
	SDL_Color fill_color;
	bool highlight;
	SDL_Color highlight_color;

};

class UI_Component_Message_Stream : public UI_Component_Generic
{
public:
	UI_Component_Message_Stream(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_NULL, int panel_name = PANEL_NULL, int button_action = BUTTON_ACTION_DO_NOTHING, SDL_Rect placement_rect = { 5,5,100,100 }, int max_lines = 20) :UI_Component_Generic(service_locator, placement_rect)
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

class UI_Component_Graphic_Button : public UI_Component_Generic
{
public:
	UI_Component_Graphic_Button(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
		spritesheet_num = SPRITESHEET_ICON;
	}

	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Init(int object_type, int template_id, SDL_Rect icon_clip = { 0,0,0,0 });

private:
	void Fetch_Sprite_Details_From_Object_ID();

	int object_type = 0;
	int template_id = 0;
	int spritesheet_num;
	SDL_Rect sprite_clip;
};