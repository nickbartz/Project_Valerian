#pragma once
#include<SDL.h>
#include<Message.h>
#include<Global_Constants.h>
#include<Message_Array.h>


class UI_Component_Generic;
class Global_Service_Locator;
class Coordinate;

class Cursor
{
public:
	Cursor(Global_Service_Locator* sPointer);
	
	SDL_Rect Get_Camera();
	SDL_Point Get_Mouse_Position();
	void Draw();

	bool left_button = false;
	bool right_button = false;
	bool left_button_previous = false;
	bool right_button_previous = false;
	int current_mouse_x = 0;
	int current_mouse_y = 0;
	int held_mouse_x = 0;
	int held_mouse_y = 0;
	int last_mouse_x = 0;
	int last_mouse_y = 0;

	void Change_Cursor_Icon(int icon_clip_x, int icon_clip_y);

	void Set_Currently_Clicked_Component(UI_Component_Generic* component);
	int Get_Recent_Mouse_Action();

	Coordinate Get_Mouse_Grid_Coord();
	SDL_Point Get_Mouse_World_Pos();

	SDL_Point Convert_Coord_To_Screen_Pos(Coordinate coordinate, bool center_of_tile = true);

	void Collect_Bus_Messages();
	void Update();

private:
	int mouse_icon_clip_x = 2 * SPRITE_SIZE;
	int mouse_icon_clip_y = 0;

	void Update_Grid_Position();
	void Parse_Input_Message(SDL_Event e);
	void Age_Mouse();

	// Camera is instantiated as a simple rect where the "w" variables is equal to the zoom;
	SDL_Rect camera = { 0,0,0,0 };

	Global_Service_Locator * service_pointer;
	UI_Component_Generic* currently_clicked_component = NULL;
	SDL_Rect drag_rect = { 0,0,0,0 };
	SDL_Rect grid_pos = { 0,0,0,0 };
};