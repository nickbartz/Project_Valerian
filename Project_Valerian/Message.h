#pragma once

using namespace std;
#include<SDL.h>
#include<string>
#include <sstream>
#include<Global_Constants.h>
#include<iostream>

class Message_Core
{
public:
	Message_Core(string message = "");
	string Get_Message_String();

	void Update_Message(string string);

protected:
	string string_message;
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

class Custom_Message : public Message_Core
{
public:
	Custom_Message(int mLength = 0, int message_array[MAX_LENGTH_CUSTOM_MESSAGE] = NULL) :Message_Core()
	{
		for (int i = 0; i < mLength; i++)
		{
			message[i] = message_array[i];
			string_message.push_back(message_array[i]);
		}

		message_length = mLength;
	}

	int Read_Message(int message_column);
	int Get_Message_Length();

private:
	int message[MAX_LENGTH_CUSTOM_MESSAGE];
	int message_length = 0;
};

// Custom Message Documentation

// General Formula: 
// Message Type --> Audience Type --> Audience Focus (specific or range) --> Message type-specific data
//
// Existing Message Types:
//
// Message for requesting that a structure update its oxygen level
// MESSAGE_TYPE_STAT_UPDATE_REQUEST --> OBJECT_TYPE_STRUCTURE --> FOCUS_TYPE_SPECIFIC --> STAT_ADJUST --> GRID_X --> GRID_Y --> STAT_STRUCTURE_OXYGEN_LEVEL --> 10
//
// Message for a tile update from the Scene Graph
// MESSAGE_TYPE_SG_TILE_UPDATE --> OBJECT_TYPE_ALL --> FOCUS_TYPE_ALL --> GRID_X --> GRID_Y --> TILE_LAYER --> STRUCTURE_TYPE --> STRUCTURE_ID
//
// Message for an Entity Create from the Scene Graph
//
// MESSAGE_TYPE_SG_ENTITY_CREATE --> OBJECT_TYPE_ALL --> FOCUS_TYPE_ALL --> GRID_X --> GRID_Y
//
// Message to set a rally point for entities 
//
// MESSAGE_TYPE_SET_RALLY_POINT --> OBJECT_TYPE_ENTITY --> FOCUS_TYPE_RANGE --> ENTITY_FACTION --> GRID_X --> GRID_Y
//
// Message to update the SG on an entities movement 
//
// MESSAGE_TYPE_SG_ENTITY_MOVEMENT -- > OBJECT_TYPE_ALL --> FOCUS_TYPE_ALL --> ENTITY_FACTION --> prevGRID_X --> prevGRID_Y --> newGRID_X --> newGRID_Y --> OBJECT_ARRAY_INDEX
//
// Message to Update the SG on a projectiles movement
//
// MESSAGE_TYPE_SG_PROJECTILE_MOVEMENT --> OBJECT_TYPE_ALL --> FOCUS_TYPE_ALL --> GRID_X --> GRID_Y  -- > PROJECTILE_FACTION --> PROJECTILE_POWER -->  PROJECTILE_RANGE --> ARRAY_INDEX