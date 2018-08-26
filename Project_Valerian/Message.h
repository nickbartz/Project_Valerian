#pragma once

using namespace std;
#include<SDL.h>
#include<string>
#include <sstream>
#include<Global_Constants.h>

class Message_Core
{
public:
	Message_Core(string message = "");
	string Get_Message_String();

	void Update_Message(string string);

private:
	string message;
};

class Message_Input: public Message_Core
{
public:
	Message_Input(SDL_Event event = {}) :Message_Core("")
	{
		sdl_event = event;
		Message_Core::Update_Message(Return_Event_String());
	}

	SDL_Event Return_Event();
	string Return_Event_String();

private:

	SDL_Event sdl_event;

};

class Message_SG_Tile_Update: public Message_Core
{
public:
	Message_SG_Tile_Update(int x  = 0, int y = 0, int layer = TILE_LAYER_BASE, int name = 0, int type = 0) :Message_Core()
	{
		grid_x = x;
		grid_y = y;
		tile_layer = layer;
		new_structure_name = name;
		new_structure_type = type;
		ostringstream oss;
		oss << "Tile at: " << x << ", " << y << " updated to type: " << type << " id: " << name;
		Update_Message(oss.str());
	}

	// Message Accessors
	int Get_Grid_X();
	int Get_Grid_Y();
	int Get_Tile_Layer();
	int Get_Structure_Name();
	int Get_Structure_Type();

private:

	int grid_x;
	int grid_y;

	int tile_layer;
	int new_structure_name;
	int new_structure_type;
};