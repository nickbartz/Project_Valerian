#pragma once
#include<SDL.h>
#include<Coordinate.h>
#include<A_Star.h>

class Global_Service_Locator;
class Object_Service_Locator;


class AI_Movement_Component
{
public:
	AI_Movement_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, SDL_Rect location);

	void Check_For_Messages();
	void Update();

	void Set_Target_Coord(Coordinate grid_point);
	void Set_Target_Pos(int pos_x, int pos_y);

	// Accessors
	SDL_Rect Return_World_Pos();
	Coordinate Return_Grid_Coord();

private:
	SDL_Rect world_pos;
	Coordinate world_coord;
	int object_speed = 3;

	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	Coordinate target_coord;
	SDL_Rect target_pos;
	vector<Tile_Queue> current_path;

};