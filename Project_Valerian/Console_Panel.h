#pragma once
#include<SDL.h>
#include<Console_Component.h>
#include<algorithm>
#include<vector>

class Global_Service_Locator;
class Object;
class Job;
class AI_Job_Component;

class UI_Panel_Generic
{
public:
	UI_Panel_Generic(Global_Service_Locator* service_locator, int window_name, SDL_Rect rect = { 0,0,100,100 });
	int Get_Panel_Name();
	bool Check_If_Click_In_Panel_Rect(SDL_Rect base_rect, SDL_Point mouse_location);
	void Change_Rect(SDL_Rect new_rect);
	bool active = false;
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Init();

protected:
	Global_Service_Locator* service_locator;
	int window;
	int panel_name;
	SDL_Rect offset_rect;
};

class UI_Panel_Console : public UI_Panel_Generic
{
public:
	UI_Panel_Console(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_PLAYER_DIAGNOSTIC, int pName = PANEL_MESSAGE_STREAM, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator,window_name, offset_rect)
	{
		panel_name = PANEL_MESSAGE_STREAM;
		message_stream = UI_Component_Message_Stream(service_locator, { offset_rect.x,offset_rect.y,offset_rect.w,offset_rect.h });
		message_stream.Set_Font_Type(FONT_SMALL);
		background_component = UI_Component_Generic(service_locator, offset_rect, true);
	}
	
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Push_Message_Into_Stream(string message);

private:
	UI_Component_Message_Stream message_stream;
	UI_Component_Generic background_component;

};

class UI_Panel_Structure_Create_Type : public UI_Panel_Generic
{
public:
	UI_Panel_Structure_Create_Type(Global_Service_Locator* service_locator = NULL, int rows = 6, int columns = 8, int window_name = WINDOW_CREATE_STRUCTURE, int panel_structure_type = PANEL_STRUCTURE_CREATE_WALL, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_name = panel_structure_type;
		panel_rows = rows;
		panel_columns = columns;
	}

	void Init(int ui_structure_type);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Handle_Mouse_Click();

private:
	int panel_buttons = 0;
	int panel_rows;
	int panel_columns; 
	vector <UI_Component_Graphic_Button> graphic_button_array;
	UI_Component_Generic background_component;
};

class UI_Panel_Object_Inventory : public UI_Panel_Generic
{
public:
	UI_Panel_Object_Inventory(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_OBJECT_DIAGNOSTIC, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_name = PANEL_OBJECT_INVENTORY;

		int horizontal_divide_a = 30;
		int horizonzal_divide_b = 70;

		item_array = UI_Component_Item_Slot_Array(service_locator, { offset_rect.x, offset_rect.y, offset_rect.w* horizontal_divide_a / 100, offset_rect.h });
		equipment_array = UI_Component_Equipment_Slot_Array(service_locator, { offset_rect.x + offset_rect.w * horizontal_divide_a / 100, offset_rect.y, offset_rect.w*(100 - horizonzal_divide_b - horizontal_divide_a) / 100, offset_rect.h });
		inventory_details = UI_Component_Object_Details_Display(service_locator, { offset_rect.x + offset_rect.w * horizonzal_divide_b / 100, offset_rect.y, offset_rect.w*(100- horizonzal_divide_b) / 100, offset_rect.h });
	}

	void Init(Object* object);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Handle_Mouse_Click();

private:
	Object * linked_object = NULL;

	int panel_buttons = 0;
	UI_Component_Item_Slot_Array item_array;
	UI_Component_Equipment_Slot_Array equipment_array;
	UI_Component_Object_Details_Display inventory_details;
	UI_Component_Generic background_component;
};

class UI_Panel_Object_Stats : public UI_Panel_Generic
{
public:
	UI_Panel_Object_Stats(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_OBJECT_DIAGNOSTIC, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_name = PANEL_OBJECT_STATS;
		background_component = UI_Component_Generic(service_locator, offset_rect, true);

		int horizontal_split = 50;

		SDL_Rect base_static_stats_rect = { offset_rect.x, offset_rect.y, offset_rect.w * horizontal_split / 100, offset_rect.h };
		SDL_Rect dynamic_stats_rect = { offset_rect.x + (horizontal_split * offset_rect.w / 100) , offset_rect.y, offset_rect.w * (100 - horizontal_split) / 100, offset_rect.h };

		base_static_stats = UI_Component_Stat_Array(service_locator, base_static_stats_rect);
		dynamic_stats = UI_Component_Stat_Array(service_locator, dynamic_stats_rect);
	}

	void Init(Object* object);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Handle_Mouse_Click();

private:
	Object * linked_object;
	UI_Component_Generic background_component;
	UI_Component_Stat_Array base_static_stats;
	UI_Component_Stat_Array dynamic_stats;

};

class UI_Panel_Object_Jobs : public UI_Panel_Generic
{
public:
	UI_Panel_Object_Jobs(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_OBJECT_DIAGNOSTIC, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_name = PANEL_OBJECT_JOBS;
	}

	void Init(Object* object);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Handle_Mouse_Click();

private:
	Object * linked_object;
	AI_Job_Component* linked_job_component;
	Job* object_current_job;
	bool reset = true;

	int num_stats = 0;
	UI_Component_Generic object_job;
	UI_Component_Message_Stream goal_list;
	UI_Component_Generic background_component;

};

class UI_Panel_Object_Production : public UI_Panel_Generic
{
public:
	UI_Panel_Object_Production(Global_Service_Locator* service_locator = NULL, int window_name = WINDOW_OBJECT_DIAGNOSTIC, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_name = PANEL_OBJECT_PRODUCTION;

		int center_vertical_divide = 60;
		int middle = center_vertical_divide;

		int center_right_vertical_divide = 50;
		int middle_right = middle + center_right_vertical_divide;

		int bottom_horizontal_divide = 90;
		int bottom_vertical_divide = 80;

		SDL_Rect blueprint_list_rect = { offset_rect.x, offset_rect.y, offset_rect.w * center_vertical_divide / 100, offset_rect.h };
		SDL_Rect blueprint_details_rect = { offset_rect.x + offset_rect.w * center_vertical_divide / 100, offset_rect.y, offset_rect.w * (100 - center_vertical_divide) / 100, offset_rect.h * bottom_horizontal_divide / 100 };
		SDL_Rect build_button_rect = { blueprint_details_rect.x, offset_rect.y + offset_rect.h * bottom_horizontal_divide / 100, blueprint_details_rect.w * bottom_vertical_divide / 100, offset_rect.h * (100 - bottom_horizontal_divide) / 100 };
		SDL_Rect production_amount_rect = { blueprint_details_rect.x + build_button_rect.w, build_button_rect.y, blueprint_details_rect.w * (100 - bottom_vertical_divide) / 100, build_button_rect.h };

		background_component = UI_Component_Generic(service_locator, offset_rect, true);
		build_button = UI_Component_Generic(service_locator, build_button_rect, true, {200,100,100,255});
		blueprint_list = UI_Component_Button_List(service_locator, blueprint_list_rect);
		blueprint_details = UI_Component_Object_Details_Display(service_locator, blueprint_details_rect);
		production_amount = UI_Component_User_Input_Box(service_locator, production_amount_rect);
	}

	void Init(Object* object);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Handle_Mouse_Click();
	void Set_Current_Blueprint_ID(int blueprint_id);

private:
	Object * linked_object = NULL;
	bool reset = true;
	int current_blueprint_id = 0;

	UI_Component_Generic background_component;
	UI_Component_Generic build_button;
	UI_Component_Button_List blueprint_list;
	UI_Component_Object_Details_Display blueprint_details;
	UI_Component_User_Input_Box production_amount;

};