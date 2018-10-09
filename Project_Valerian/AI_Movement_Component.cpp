#include <AI_Movement_Component.h>
#include<Service_Locator.h>
#include<Message_Array.h>
#include<Coordinate.h>
#include<SDL.h>
#include<AI_Stats_Component.h>
#include<AI_Rel_Component.h>
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
	// MOVE ONLY IF THE OBJECT IS AN ENTITY
	if (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type() == OBJECT_TYPE_ENTITY)
	{	
		// CALCULATE DELTAS
		int delta_x = target_pos.x - world_pos.x;
		int delta_y = target_pos.y - world_pos.y;

		if (delta_x != 0 || delta_y != 0)
		{

			// ACTUALLY MOVE ENTITY
			if (delta_x > 0) world_pos.x += min(object_speed, delta_x);
			else if (delta_x < 0) world_pos.x += max(-object_speed, delta_x);

			if (delta_y > 0) world_pos.y += min(object_speed, delta_y);
			else if (delta_y < 0) world_pos.y += max(-object_speed, delta_y);

			// CHANGE ANIMATIONS FOR ENTITY BASED ON MOVEMENT
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
		// If there is no difference between the target position and the current position, it must mean that the entity has moved to the next coordinate in the path already
		// consequently the path has to be updated to reflect this and a new target assigned
		else if (current_path.size() != 0)
		{
			Coordinate previous_world_coord = { world_coord.x, world_coord.y };
			Coordinate next_step = { current_path.back().x_tile,current_path.back().y_tile };
			Set_Target_Pos(next_step.x * TILE_SIZE, next_step.y * TILE_SIZE);
			world_coord = { next_step.x, next_step.y };
			current_path.pop_back();

			// We need to send a message to the message bus that the entity has shifted target tiles
			int faction = object_locator->Return_AI_Rel_Pointer()->Return_Object_Faction();
			int uniq_id = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_UNIQ_ID);
			int custom_message[9] = {MESSAGE_TYPE_SG_ENTITY_MOVEMENT,OBJECT_TYPE_ANY, FOCUS_ALL,faction, previous_world_coord.x, previous_world_coord.y, world_coord.x, world_coord.y,uniq_id};
			service_locator->get_MB_Pointer()->Add_Custom_Message(9, custom_message);
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
	if (!service_locator->get_Scene_Graph()->Tile_Is_Inaccessible(grid_coord, object_locator->Return_AI_Rel_Pointer()->Return_Object_Faction()))
	{
		target_coord = grid_coord;
		current_path = service_locator->get_Pathfinder()->pathFind(world_coord, target_coord, 1000, object_locator->Return_AI_Rel_Pointer()->Return_Object_Faction());

		if (current_path.size() != 0)
		{
			Coordinate next_step = { current_path.back().x_tile,current_path.back().y_tile };
			Set_Target_Pos(next_step.x * TILE_SIZE, next_step.y * TILE_SIZE);
			current_path.pop_back();
		}
	}
}