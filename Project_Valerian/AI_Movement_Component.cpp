#include <AI_Movement_Component.h>
#include<Service_Locator.h>
#include<Message_Array.h>
#include<Coordinate.h>
#include<SDL.h>
#include<AI_Stats_Component.h>
#include<algorithm>
#include<Scene_Graph.h>


AI_Movement_Component::AI_Movement_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, SDL_Rect location)
{
	service_locator = sLocator;
	object_locator = oLocator;

	world_pos = location;
	world_coord = { location.x / TILE_SIZE, location.y / TILE_SIZE };

	target_pos = location;
}

void AI_Movement_Component::Check_For_Messages()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		switch (service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(0))
		{
		case MESSAGE_TYPE_SET_ENTITY_RALLY_POINT:
			if (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type() == OBJECT_TYPE_ENTITY)
			{
				Set_Target_Coord({ service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(4), service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(5) });
			}
			break;
		}
	}
}

void AI_Movement_Component::Update()
{
	if (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type() == OBJECT_TYPE_ENTITY)
	{	
		int delta_x = target_pos.x - world_pos.x;
		int delta_y = target_pos.y - world_pos.y;

		if (delta_x != 0 || delta_y != 0)
		{
			if (delta_x > 0) world_pos.x += min(object_speed, delta_x);
			else if (delta_x < 0) world_pos.x += max(-object_speed, delta_x);

			if (delta_y > 0) world_pos.y += min(object_speed, delta_y);
			else if (delta_y < 0) world_pos.y += max(-object_speed, delta_y);

			if (abs(delta_x) >= abs(delta_y))
			{
				if (delta_x >= 0) object_locator->Return_Render_Pointer()->Change_Entity_Current_Animation(ANIM_WALK_RIGHT);
				else object_locator->Return_Render_Pointer()->Change_Entity_Current_Animation(ANIM_WALK_LEFT);
			}
			else
			{
				if (delta_y >= 0) object_locator->Return_Render_Pointer()->Change_Entity_Current_Animation(ANIM_WALK_DOWN);
				else object_locator->Return_Render_Pointer()->Change_Entity_Current_Animation(ANIM_WALK_UP);
			}

			object_locator->Return_Render_Pointer()->Increment_Entity_Animation();
		}
		else if (current_path.size() != 0)
		{
			Coordinate next_step = { current_path.back().x_tile,current_path.back().y_tile };
			Set_Target_Pos(next_step.x * TILE_SIZE, next_step.y * TILE_SIZE);
			world_coord = { next_step.x, next_step.y };
			current_path.pop_back();
		}
	}
}

SDL_Rect AI_Movement_Component::Return_World_Pos()
{
	return world_pos;
}

Coordinate AI_Movement_Component::Return_Grid_Coord()
{
	return world_coord;
}

void AI_Movement_Component::Set_Target_Pos(int pos_x, int pos_y)
{
	target_pos.x = pos_x;
	target_pos.y = pos_y;
}

void AI_Movement_Component::Set_Target_Coord(Coordinate grid_coord)
{
	if (!service_locator->get_Scene_Graph()->Tile_Is_Inaccessible(grid_coord))
	{
		target_coord = grid_coord;
		current_path = service_locator->get_Pathfinder()->pathFind(world_coord, target_coord, 1000);

		if (current_path.size() != 0)
		{
			Coordinate next_step = { current_path.back().x_tile,current_path.back().y_tile };
			Set_Target_Pos(next_step.x * TILE_SIZE, next_step.y * TILE_SIZE);
			current_path.pop_back();
		}
	}
}