#pragma once

#include<Global_Constants.h>
#include<Draw_System.h>
#include<SDL.h>
#include<vector>	
#include<AI_Item_Component.h>

class Cursor;
class Global_Service_Locator;

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
	void Set_Highlight(bool highlight);
	void Update_Component_Title(string new_title, SDL_Color text_color = {}, int alignment = 1);

	// Accessors
	SDL_Rect Return_Rect();
	void Change_Offset_Rect(int argument, int new_value);
	void Increment_Offset_Rect(int argument, int increment);
	int Get_Font_Type();
	string Get_Title();
	int Get_Panel();
	int Get_Window();
	int Get_Message_Length();
	SDL_Rect Get_Offset_Rect();
	vector<int> Get_Message_In_Vector();

	// Functions that check for clicks
	bool Check_If_Click_In_Component_Rect(SDL_Point cursor_position);
	bool Currently_Clicked(); // return current state
	bool Currently_Focus();
	bool Currently_Clicked(bool state_override); // return current state and also set current state
	bool Currently_Focus(bool state_override);
	void Handle_Button_Click();

	// Base draw_functions for the component
	void Draw(SDL_Rect base_rect);



protected:

	Global_Service_Locator * service_locator;

	// Variables relating to the component if it is a simple button
	int font_type = FONT_DEFAULT;
	bool draw_title = false;
	string title = "";
	SDL_Rect text_offset = { 0,0,25,25 };
	SDL_Color text_color;

	struct text_row
	{
		SDL_Rect row_rect;
		string row_text;
	};

	bool text_is_multirow = false;
	vector<text_row> multi_row_text;
	
	// Variables related to the function of the component if it is a button // TO BE DEPRECATED
	int window = WINDOW_NULL;
	int panel = PANEL_NULL;
	int button_action = BUTTON_ACTION_DO_NOTHING;
	bool currently_clicked = false;
	bool currently_is_focus = false;

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
		max_message_lines = placement_rect.h / 15;

		font_type = FONT_SMALL;
	}

	void Set_Message_At_Array_Num(string new_message, int array_num);
	void Set_Bold_Line(int bold_line);
	void Push_Message_Into_Stream(string new_message);

	void Clear_All_Messages_From_Stream();
	void Draw(SDL_Rect base_rect);

private:
	int max_message_lines = 30;
	int bold_line = -1;

	int num_messages_in_stream = 0;
	string message_array[UI_MAX_CONSOLE_MESSAGES];
	string console_draw_string;

	void Create_String_From_Message_Array();

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
	Item_Slot* Return_Slot_Pointer();

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
	void Make_Bar();

private:
	Object * reference_object;
	AI_Stats_Component* ai_stats = NULL;
	int stat_name;
	string stat_string_name;
	int string_width_offset = 150;
	int current_stat_value = 0;
	int max_stat_value = 0;
	SDL_Rect text_rect;
	SDL_Rect stat_rect;

	bool is_bar = false;
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
	UI_Component_Generic* Handle_Click_On_Component();
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

class UI_Component_Object_Details_Display : public UI_Component_Generic
{
public:
	UI_Component_Object_Details_Display(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
		int title_horizontal_offset = 15;
		int title_vertical_offset = 15;
		if (placement_rect.h > 0)
		{
			title_vertical_offset = (placement_rect.w*title_horizontal_offset / placement_rect.h);
		}
		
		int requirements_horizontal_offset = 15;
		int requirements_vertical_offset = 85;

		int x_offset = placement_rect.w * requirements_horizontal_offset / 100;

		SDL_Rect object_icon_rect = {offset_rect.x, offset_rect.y, offset_rect.w * title_horizontal_offset/100, offset_rect.h * title_vertical_offset/100};
		SDL_Rect object_title_rect = { offset_rect.x + offset_rect.w * title_horizontal_offset / 100, offset_rect.y, offset_rect.w * (100 - title_horizontal_offset)/100, offset_rect.h * title_vertical_offset / 100 };
		SDL_Rect object_description_rect = {offset_rect.x, offset_rect.y + offset_rect.h * title_vertical_offset / 100, offset_rect.w, offset_rect.h * requirements_vertical_offset / 100 - object_title_rect.h};
		SDL_Rect object_requirement_rect = { offset_rect.x, offset_rect.y + offset_rect.h * requirements_vertical_offset / 100, offset_rect.w * requirements_horizontal_offset / 100, offset_rect.h * (100 - requirements_vertical_offset)/100 };

		object_icon = UI_Component_Graphic_Button(service_locator, object_icon_rect);
		object_title = UI_Component_Generic(service_locator, object_title_rect);
		object_description = UI_Component_Generic(service_locator, object_description_rect);
		object_description.Set_Highlight(false);

		for (int i = 0; i < MAX_ITEMS_PER_BLUEPRINT; i++)
		{
			object_requirements[i] = UI_Component_Item_Slot_Button(service_locator, object_requirement_rect);
			object_requirement_rect.x += x_offset;
		}

	}
	void Draw(SDL_Rect base_rect);
	void Init(Object* owner_object, int object_type = OBJECT_TYPE_NULL, int object_template_id = 0);

private:
	UI_Component_Graphic_Button object_icon;
	UI_Component_Generic object_title;
	UI_Component_Generic object_description;
	UI_Component_Item_Slot_Button object_requirements[MAX_ITEMS_PER_BLUEPRINT];
	Item_Slot current_requirement_slots[MAX_ITEMS_PER_BLUEPRINT];

	int num_requirements = 0;
};

class UI_Component_User_Input_Box : public UI_Component_Generic
{
public:
	UI_Component_User_Input_Box(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
	}

	void Draw(SDL_Rect base_rect);
	void Handle_Click_On_Component();
	void Init(bool just_num);
	void Check_For_User_Input();
	int Get_Current_Num();

private:
	string text_input;
	int text_width;
	int text_height;
	
	// If the box is just for numbers
	bool is_num = false;
	int number = 0;



	void Add_Char_To_Box(char* input);
	void Remove_Char_From_Box();
};

class UI_Component_Item_Slot_Array :public UI_Component_Generic
{
public:
	UI_Component_Item_Slot_Array(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{
	}
	void Draw(SDL_Rect base_rect);
	Item_Slot* Handle_Click_On_Component();
	void Init(Item_Slot item_slot_array[], int num_item_slots);

private:
	int num_item_slots;
	int num_columns = 5;
	int num_rows = 8;
	int num_panel_buttons = 0;
	vector <UI_Component_Item_Slot_Button> item_slot_array;
};

class UI_Component_Equipment_Slot_Array :public UI_Component_Generic
{
public:
	UI_Component_Equipment_Slot_Array(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{

	}
	void Draw(SDL_Rect base_rect);
	Item_Slot* Handle_Click_On_Component();
	void Init(Item_Slot item_slot_array[], int num_item_slots);

private:
};

class UI_Component_Stat_Array :public UI_Component_Generic
{
public:
	UI_Component_Stat_Array(Global_Service_Locator* service_locator = NULL, SDL_Rect placement_rect = { 0,0,0,0 }) :UI_Component_Generic(service_locator, placement_rect, false)
	{

	}

	void Draw(SDL_Rect base_rect);
	Item_Slot* Handle_Click_On_Component();
	void Init(Object* linked_object);

	void Add_Stat_Button(int stat_name, string stat_string_name, bool is_bar = false);

private:
	Object * linked_object;
	int num_stat_buttons = 0;
	vector<UI_Component_Stat_Button> stat_buttons;
	
	void Adjust_Stat_Button_Positioning();
};





