#pragma once

#include<Global_Constants.h>
#include<Draw_System.h>
#include<SDL.h>
#include<vector>	

class Cursor;
class Global_Service_Locator;
class Item_Slot;

// Simpler Components

class UI_Component_Generic
{
public:
	UI_Component_Generic(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 5,5,100,100 }, bool component_is_filled = false, SDL_Color bColor = { 50,50,50,200 }, bool is_highlight = true, SDL_Color hColor = { 255,255,255,255 });

	void Init();

	// Functions that determine what sort component this becomes
	void Set_Click_Message(int message_length, int message_array[]);
	void Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor);
	void Assign_Window(int window_name);
	void Assign_Panel(int panel_name);
	void Set_Font_Type(int font_type);
	void Set_Background_Color(SDL_Color new_color);
	void Change_Component_Title(string new_title, SDL_Rect text_offset_rect = {}, SDL_Color text_color = {});

	// Accessors
	SDL_Rect Return_Rect();
	void Change_Offset_Rect(int argument, int new_value);
	void Increment_Offset_Rect(int argument, int increment);
	int Get_Font_Type();
	string Get_Title();
	int Get_Panel();
	int Get_Window();

	// Functions that check for clicks
	bool Check_If_Click_In_Component_Rect(SDL_Point cursor_position);
	bool Currently_Clicked(); // return current state
	bool Currently_Clicked(bool state_override); // return current state and also set current state
	void Handle_Button_Click();

	// Base draw_functions for the component
	void Draw(SDL_Rect base_rect);

protected:

	Global_Service_Locator * service_locator;

	// Variables relating to the component if it is a simple button
	bool draw_title = false;
	string title= "";
	SDL_Rect text_offset = { 0,0,25,25 };
	int font_type = FONT_DEFAULT;
	SDL_Color text_color;
	bool bevel = false;

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
	UI_Component_Message_Stream(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 5,5,100,100 }) :UI_Component_Generic(service_locator, placement_rect)
	{
		max_message_lines = placement_rect.h / 20;

		for (int i = 0; i < UI_MAX_CONSOLE_MESSAGES; i++)
		{
			message_array[i] = "-";
		}
	}

	void Set_Message_At_Array_Num(string new_message, int array_num);
	void Set_Bold_Line(int bold_line);
	void Push_Message_Into_Stream(string new_message);
	void Clear_All_Messages_From_Stream();
	void Draw(SDL_Rect base_rect);

private:
	int max_message_lines;
	int bold_line = -1;

	string message_array[UI_MAX_CONSOLE_MESSAGES];

};

class UI_Component_Graphic_Button : public UI_Component_Generic
{
public:
	UI_Component_Graphic_Button(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
		spritesheet_num = SPRITESHEET_ICON;
	}

	void Draw(SDL_Rect base_rect);
	void Init(int object_type, int template_id, SDL_Rect icon_clip = { 0,0,0,0 });

private:
	void Fetch_Sprite_Details_From_Object_ID();
	bool sprite_clip_override = false;
	int object_type = 0;
	int template_id = 0;
	int spritesheet_num;
	SDL_Rect sprite_clip;
};

class UI_Component_Item_Slot_Button : public UI_Component_Generic
{
public:
	UI_Component_Item_Slot_Button(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
		spritesheet_num = SPRITESHEET_ICON;
	}

	void Draw(SDL_Rect base_rect);
	void Init(Item_Slot* slot_pointer);

private:
	Item_Slot * slot_pointer;
	int spritesheet_num;
	SDL_Rect sprite_clip;
};

class UI_Component_Stat_Button : public UI_Component_Generic
{
public:
	UI_Component_Stat_Button(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
	}
	void Draw(SDL_Rect base_rect);
	void Init(Object* reference_object, int stat_name, string stat_s_name);

private:
	Object * reference_object;
	int stat_name;
	string stat_string_name;
	int string_width_offset = 90;
};

// Complex Modular Components //

class UI_Component_Scroll_Bar : public UI_Component_Generic
{
public:
	UI_Component_Scroll_Bar(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{

	}

	void Draw(SDL_Rect base_rect);
	void Init(int length, int button_height, int num_objects_displayed_at_once);
	void Update_Scroll_Bar(int num_elements_in_array);
	int Handle_Click_On_Component();

private:
	int button_height;
	int scroll_bar_length;
	int current_object_index;
	int num_scroll_elements;
	int num_objects_displayed_at_once;

	int increment_distance;

	UI_Component_Graphic_Button up_arrow;
	UI_Component_Graphic_Button down_arrow;
	UI_Component_Generic current_location_indicator;
	UI_Component_Generic current_location_range;
};

class UI_Component_Button_List : public UI_Component_Generic
{
public:
	UI_Component_Button_List(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
		num_buttons_to_display = 10;
		button_height = placement_rect.h / num_buttons_to_display;
		scroll_bar_width = button_height;
		scroll_bar = UI_Component_Scroll_Bar(service_locator, { placement_rect.w - scroll_bar_width, placement_rect.y, scroll_bar_width,placement_rect.h });

	}
	void Draw(SDL_Rect base_rect);
	void Handle_Click_On_Component();
	void Add_Button_To_Button_List(UI_Component_Generic new_button);
	void Init();

private:
	int scroll_bar_width = 0;
	int button_height = 0;
	int current_button_index = 0;
	int num_buttons_to_display = 0;
	void Adjust_Button_List_Buttons();

	vector<UI_Component_Generic> button_list;
	UI_Component_Scroll_Bar scroll_bar;

};

class UI_Component_Details_Display : public UI_Component_Generic
{
public:
	UI_Component_Details_Display(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
	}
	void Draw(SDL_Rect base_rect);
	void Init();

private:

};

class UI_Component_User_Input_Box : public UI_Component_Generic
{
public:
	UI_Component_User_Input_Box(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
	}
	void Draw(SDL_Rect base_rect);
	void Collect_Input_Messages();
	void Init();

private:
	string text_input = "test";

	void Add_Char_To_Box(char* input);
};






