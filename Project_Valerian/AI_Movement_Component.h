#pragma once
#include<SDL.h>
#include<Coordinate.h>
#include<A_Star.h>

class Global_Service_Locator;
class Object_Service_Locator;


class AI_Movement_Component
{
public:
	AI_Movement_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, SDL_Rect location, int object_type, int object_template_id);

	void Check_For_Messages();
	void Update();
	void Update_Entity();
	void Update_Container();
	void Update_Projectile();

	void Set_Target_Coord(Coordinate grid_point);
	void Set_Target_Pos(int pos_x, int pos_y);
	void Set_Projectile_Velocity(SDL_Point target);
	void Stop_Moving();

	// Accessors
	SDL_Rect Return_World_Pos();
	Coordinate Return_Grid_Coord();

private:
	SDL_Rect world_pos;
	Coordinate world_coord = { 0,0 };
	int object_type = 0;

	bool Update_World_Coord(SDL_Rect new_world_rect);

	int object_base_speed = 3;
	int obj_x_vel = 0.0; // For projectiles
	int obj_y_vel = 0.0; // For projectiles

	// This is extra speed above an integer number that gets lost in the conversion to pixel movemnet, we save it for reference
	double vel_remainder_x = 0.0; // Needed to account for the fact that screen position is an int-based system, and our velocity is a double;
	double vel_remainder_y = 0.0; 

	// We store the cumulative movement that would have occured if we weren't converting speed to an int and then add it back in once it gets >= 1
	double cumulative_remainder_x = 0.0;
	double cumulative_remainder_y = 0.0;

	Global_Service_Locator* service_locator;
	Object_Service_Locator* object_locator;

	Coordinate target_coord;
	SDL_Rect target_pos;
	vector<Tile_Queue> current_path;

	int bounce_direction = 0;
	int current_bounce = 0;
	int bounce_max = 0;
	int bounce_delay = 0;
	int max_bounce_delay = 5;

};