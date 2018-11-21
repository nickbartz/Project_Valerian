#pragma once

#include<SDL.h>
#include<Console_Component.h>
#include<Console_Panel.h>

class Cursor;
class Object;

class UI_Window_Generic
{
public:
	UI_Window_Generic(Global_Service_Locator* service_locator, int window_name, string window_title, SDL_Rect console_window_rect);

	void Draw(Draw_System* draw_system);
	void Change_Rect(SDL_Rect new_rect);
	SDL_Rect Return_Rect();
	void Change_Window_Name(string new_name);

	bool is_open();
	bool is_currently_clicked();

	void Init();

protected:

	Global_Service_Locator* service_locator;

	void Create_Panel_Header(SDL_Rect pos_rect, int panel_name, string panel_title, SDL_Rect text_offset, SDL_Color text_color);
	void Size_Panel_Headers();
	void Move_Window(Cursor* cursor);

	void Handle_Mouse_Click(Cursor* cursor);

	
	UI_Component_Generic window_title_bar;
	UI_Component_Generic window_close_button;
	int window_title_height = 25;
	int panel_bar_height = 25;

	int currently_active_panel = PANEL_NULL;

	int active_panel_headers = 0;
	UI_Component_Generic panel_headers[UI_MAX_WINDOW_HEADERS];
	SDL_Rect base_window_rect;

	int window_name;
	bool currently_open = true;
};

class UI_Window_Player_Diagnostic :public UI_Window_Generic
{
public:
	UI_Window_Player_Diagnostic(Global_Service_Locator* service_locator = NULL, SDL_Rect base_rect = { 100,100,300,500 }) :UI_Window_Generic(service_locator, WINDOW_PLAYER_DIAGNOSTIC, "Diagnostics", base_rect)
	{
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_MESSAGE_STREAM, "Console", { 5,3,0,0 }, { 255,255,255,255 });

		Size_Panel_Headers();

		panel_message_panel = UI_Panel_Console(service_locator, window_name, PANEL_MESSAGE_STREAM, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		panel_message_panel.active = true;

		currently_active_panel = PANEL_MESSAGE_STREAM;
	}

	void Handle_Mouse_Click(Cursor* cursor);
	void Draw(Draw_System* draw_system);
	void Push_Message_Into_Stream(string message);

private:
	UI_Panel_Console panel_message_panel;
};

class UI_Window_Structure_Create :public UI_Window_Generic
{
public:
	UI_Window_Structure_Create(Global_Service_Locator* service_locator = NULL, SDL_Rect base_rect = { 400,100,SPRITE_SIZE*5,SPRITE_SIZE*5 }) :UI_Window_Generic(service_locator, WINDOW_CREATE_STRUCTURE, "Structure Create", base_rect)
	{
		// Create Wall Structure Menu Panels
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_STRUCTURE_CREATE_WALL, "Wall", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_STRUCTURE_CREATE_FLOOR, "Floor", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_STRUCTURE_CREATE_MISC, "Misc", { 5,3,0,0 }, { 255,255,255,255 });

		structure_create_wall = UI_Panel_Structure_Create_Type(service_locator, 5, 5, WINDOW_CREATE_STRUCTURE, PANEL_STRUCTURE_CREATE_WALL, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		structure_create_floor = UI_Panel_Structure_Create_Type(service_locator, 5, 5, WINDOW_CREATE_STRUCTURE, PANEL_STRUCTURE_CREATE_FLOOR, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		structure_create_misc = UI_Panel_Structure_Create_Type(service_locator, 5, 5, WINDOW_CREATE_STRUCTURE, PANEL_STRUCTURE_CREATE_MISC, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });

		Size_Panel_Headers();

		currently_active_panel = PANEL_STRUCTURE_CREATE_WALL;
	}

	void Init();

	void Handle_Mouse_Click(Cursor* cursor);
	void Draw(Draw_System* draw_system);

private:
	UI_Panel_Structure_Create_Type structure_create_wall;
	UI_Panel_Structure_Create_Type structure_create_floor;
	UI_Panel_Structure_Create_Type structure_create_misc;
};

class UI_Window_Object_Diagnostic :public UI_Window_Generic
{
public:
	UI_Window_Object_Diagnostic(Global_Service_Locator* service_locator = NULL, SDL_Rect base_rect = {0,0,SPRITE_SIZE*20,SPRITE_SIZE*10}) :UI_Window_Generic(service_locator, WINDOW_OBJECT_DIAGNOSTIC, "Entity Diagnostic", base_rect)
	{
		// Create Wall Structure Menu Panels
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_OBJECT_INVENTORY, "Inventory", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_OBJECT_STATS, "Stats", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_OBJECT_JOBS, "Jobs", { 5,3,0,0 }, { 255,255,255,255 });
		Create_Panel_Header({ 0,window_title_height,30,panel_bar_height }, PANEL_OBJECT_PRODUCTION, "Production", { 5,3,0,0 }, { 255,255,255,255 });

		object_inventory = UI_Panel_Object_Inventory(service_locator, 6, 8, WINDOW_OBJECT_DIAGNOSTIC, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		object_stats = UI_Panel_Object_Stats(service_locator, WINDOW_OBJECT_DIAGNOSTIC, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		object_jobs = UI_Panel_Object_Jobs(service_locator, WINDOW_OBJECT_DIAGNOSTIC, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });
		object_production = UI_Panel_Object_Production(service_locator, WINDOW_OBJECT_DIAGNOSTIC, { 0,window_title_height + panel_bar_height,base_rect.w,base_rect.h - window_title_height - panel_bar_height });

		Size_Panel_Headers();

		currently_active_panel = PANEL_OBJECT_INVENTORY;

		currently_open = false;
	}

	void Init(int base_x, int base_y, Object* diagnostic_object);
	void Handle_Mouse_Click(Cursor* cursor);
	void Draw(Draw_System* draw_system);

private:
	UI_Panel_Object_Inventory object_inventory;
	UI_Panel_Object_Stats object_stats;
	UI_Panel_Object_Jobs object_jobs;
	UI_Panel_Object_Production object_production;
};

class UI_Window_Screen_Buttons : public UI_Window_Generic
{
public:
	UI_Window_Screen_Buttons(Global_Service_Locator* service_locator = NULL, SDL_Rect base_rect = { 0,SCREEN_HEIGHT - 5*SPRITE_SIZE,SPRITE_SIZE * 1,SPRITE_SIZE * 5 }):UI_Window_Generic(service_locator, WINDOW_SCREEN_BUTTONS, "Actions", base_rect)
	{
		SDL_Rect delete_structure_rect = { 0,0,TILE_SIZE,TILE_SIZE };
		UI_Static_Button_Delete_Structure = UI_Component_Graphic_Button(service_locator, delete_structure_rect);
		SDL_Rect rally_point_rect = { 0,TILE_SIZE,TILE_SIZE,TILE_SIZE };
		UI_Static_Button_Set_Rally_Point = UI_Component_Graphic_Button(service_locator, rally_point_rect);
		SDL_Rect mining_button_rect = { 0,2*TILE_SIZE,TILE_SIZE,TILE_SIZE };
		UI_Static_Button_Mark_Asteroid_For_Mining = UI_Component_Graphic_Button(service_locator, mining_button_rect);
	}

	void Init();
	void Handle_Mouse_Click();
	void Draw();

private:
	UI_Component_Graphic_Button UI_Static_Button_Delete_Structure;
	UI_Component_Graphic_Button UI_Static_Button_Set_Rally_Point;
	UI_Component_Graphic_Button UI_Static_Button_Mark_Asteroid_For_Mining;
};