#pragma once
#include<SDL.h>
#include<Console_Component.h>
#include<algorithm>
#include<vector>

class Global_Service_Locator;
class Object;

class UI_Panel_Generic
{
public:
	UI_Panel_Generic(Global_Service_Locator* service_locator, int window_name, SDL_Rect rect = { 0,0,100,100 });
	int Get_Panel_Name();
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
		panel_name = pName;
		
		message_stream = UI_Component_Message_Stream(service_locator, window, panel_name, BUTTON_ACTION_DO_NOTHING, { offset_rect.x,offset_rect.y,offset_rect.w,offset_rect.h }, min(UI_MAX_CONSOLE_MESSAGES, offset_rect.h / 20));
		message_stream.Set_Font_Type(FONT_SMALL);
	}
	
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Push_Message_Into_Stream(string message);

private:
	UI_Component_Message_Stream message_stream;

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
	void Check_For_Clicks();

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
	UI_Panel_Object_Inventory(Global_Service_Locator* service_locator = NULL, int rows = 6, int columns = 8, int window_name = WINDOW_OBJECT_DIAGNOSTIC, SDL_Rect offset_rect = { 0,0,100,100 }) : UI_Panel_Generic(service_locator, window_name, offset_rect)
	{
		panel_rows = rows;
		panel_columns = columns;
		panel_name = PANEL_OBJECT_INVENTORY;
	}

	void Init(Object* object);
	void Draw(Draw_System* draw_system, SDL_Rect base_rect);
	void Check_For_Clicks();

private:
	int panel_buttons = 0;
	int panel_rows;
	int panel_columns;
	vector <UI_Component_Graphic_Button> graphic_button_array;
	UI_Component_Generic background_component;
};