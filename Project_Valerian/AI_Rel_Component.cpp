#include <AI_Rel_Component.h>
#include<Service_Locator.h>
#include<Message_Array.h>
#include<Global_Constants.h>
#include<AI_Stats_Component.h>
#include<AI_Movement_Component.h>
#include<Object.h>
#include<Scene_Graph.h>

AI_Rel_Component::AI_Rel_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator)
{
	service_locator = sLocator;
	object_locator = oLocator;
	object_type = object_locator->Return_AI_Stats_Pointer()->Return_Object_Type();
}

void AI_Rel_Component::Check_For_Messages()
{
	Message_Array* mb_pointer = service_locator->get_MB_Pointer();
	for (int i = 0; i < mb_pointer->count_custom_messages; i++)
	{
		Custom_Message* message = &mb_pointer->Custom_Message_Array[i];
		switch (message->Read_Message(0))
		{
		case MESSAGE_TYPE_SG_PROJECTILE_MOVEMENT:
		{
			if (Check_Object_Range({ message->Read_Message(3), message->Read_Message(4)}) == 0)
			{
				Object* new_projectile = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(OBJECT_TYPE_PROJECTILE, message->Read_Message(7));
				Add_Projectile_Relationship(new_projectile, message->Read_Message(6));
			}
		}
			break;
		}
	}
}

int AI_Rel_Component::Check_Object_Range(Coordinate coord)
{
	int dif_x = abs(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord().x - coord.x);
	int dif_y = abs(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord().y - coord.y);

	return(max(dif_x, dif_y));
}

bool AI_Rel_Component::Check_If_Object_Is_Dead(int rel_array_num)
{
	if (Obj_Relationship_Array[rel_array_num].object_pointer->Get_Assigned_Flag() == OBJECT_UNASSIGNED || Obj_Relationship_Array[rel_array_num].object_pointer->Get_Uniq_ID() != Obj_Relationship_Array[rel_array_num].object_uniq_id)
	{
		return true;
	}
	else return false;
}

int AI_Rel_Component::Add_Object_Relationship(Object* object)
{
	for (int i = 0; i < AI_REL_MAX_OBJECT_RELATIONSHIPS; i++)
	{
		if (Obj_Relationship_Array[i].object_uniq_id == 0)
		{
			Obj_Relationship_Array[i] = { object->Get_Uniq_ID(), object };
			current_num_object_relationships++;
			return i;
		}
	}

	// If a spot cannot be found in the array, return -1 to signal that there is no room for more relationships
	return -1;

}

void AI_Rel_Component::Remove_Object_Relationship(int array_num)
{
	Obj_Relationship_Array[array_num].object_uniq_id = 0;
	current_num_object_relationships--;
}

void AI_Rel_Component::Update()
{
	if (current_num_projectile_relationships > 0) Update_Projectile_Relationships();
}

void AI_Rel_Component::Add_Projectile_Relationship(Object* projectile, int projectile_temp_num)
{
	int array_pos = Add_Object_Relationship(projectile);

	if (array_pos >= 0)
	{
		Projectile_Relationship new_proj_rel = { 1, array_pos,0,projectile_temp_num };

		for (int i = 0; i < AI_REL_MAX_PROJECTILE_RELATIONSHIPS; i++)
		{
			if (projectile_rel_array[i].init == 0)
			{
				projectile_rel_array[i] = new_proj_rel;
				current_num_projectile_relationships++;
				return;
			}
		}
	}
	else
	{
		return;
	}
}

void AI_Rel_Component::Remove_Projectile_Relationship(int projectile_array_num)
{
	Remove_Object_Relationship(projectile_rel_array[projectile_array_num].rel_array_num);
	projectile_rel_array[projectile_array_num].init = 0;
	current_num_projectile_relationships--;
}

void AI_Rel_Component::Update_Projectile_Relationships()
{
	for (int i = 0; i < AI_REL_MAX_PROJECTILE_RELATIONSHIPS; i++)
	{
		if (projectile_rel_array[i].init == 1)
		{
			// Check to see if there are any now dead objects active in the array 
			if (Check_If_Object_Is_Dead(projectile_rel_array[i].rel_array_num))
			{
				Remove_Projectile_Relationship(i);
			}
			else
			{
				// Remove any projectiles that are no longer in range
				if (Check_Object_Range(Obj_Relationship_Array[projectile_rel_array[i].rel_array_num].object_pointer->get_coordinate()) > 1)
				{
					Remove_Projectile_Relationship(i);
				}
				// Take damage from any projectile that is of a different faction than your own
				else if (projectile_rel_array[i].damage_taken == 0)
				{
					int projectile_power = service_locator->get_Game_Library()->Fetch_Projectile_Template(projectile_rel_array[i].projectile_template_num)->projectile_power;
					object_locator->Return_AI_Stats_Pointer()->Adjust_Stat(STAT_OBJECT_HEALTH, -projectile_power);
					projectile_rel_array[i].damage_taken = projectile_power;
					Remove_Projectile_Relationship(i);
					Obj_Relationship_Array[projectile_rel_array[i].rel_array_num].object_pointer->Set_Assigned_Flag(OBJECT_UNASSIGNED);
				}
			}

		}
	}
}
