#include<AI_Job_Component.h>
#include<iostream>
#include<Coordinate.h>
#include<map>
#include<Scene_Graph.h>
#include<AI_Stats_Component.h>
#include<Message_Array.h>
#include<AI_Movement_Component.h>
#include<AI_Rel_Component.h>
#include<AI_Item_Component.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<Job.h>

// Instantiation

AI_Job_Component::AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int object_type, int template_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	Clear_All_Goals_From_Array();

	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		{int job_id = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(template_id)->job_type;
			if (job_id != 0)
			{
				active_job = service_locator->get_Scene_Graph()->Job_Create_Local_Structure_Template_Job(object_locator->Return_Object_Pointer(), job_id);
				if (current_num_job_goals == 0)
				{
					Query_Job_For_Goals();
				}
			}
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	case OBJECT_TYPE_CONTAINER:
		break;
	}
}

void AI_Job_Component::free()
{
}

// ACCESSORS

Job* AI_Job_Component::Return_Current_Job_Pointer()
{
	if (active_job != NULL) return active_job;
	else return NULL;
}

int AI_Job_Component::Return_Current_Num_Goals()
{
	return current_num_job_goals;
}

Job_Goal* AI_Job_Component::Return_Goal_At_Array_Num(int array_num)
{
	if (current_goals.size() > array_num) return &current_goals[array_num];
}

int AI_Job_Component::Return_Current_Goal_Index()
{
	return next_goal_index;
}

// HIGHER LEVEL OBJECT AI FUNCTIONS
// Update Cycle

void AI_Job_Component::Update()
{
	switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
	{
	case OBJECT_TYPE_STRUCTURE:
		Update_Structure();
		break;
	case OBJECT_TYPE_ENTITY:
		Update_Entity();
		break;
	}

}

void AI_Job_Component::Update_Structure()
{
	if (current_num_job_goals > 0) Process_Next_Goal();
}

void AI_Job_Component::Update_Entity()
{
	Update_Entity_Manage_Jobs();
}

void AI_Job_Component::Update_Entity_Manage_Jobs()
{
	if (Is_Current_Job_Still_Relevant() == true)
	{
		if (current_num_job_goals > 0 && next_goal_index < current_num_job_goals)
		{
			Process_Next_Goal();
		}
		else if (active_job->Return_Num_Unassigned_Goalsets() > 0)
		{
			Query_Job_For_Goals();
		}
	}
	else
	{
		Update_Entity_Choose_New_Job();
	}
}

void AI_Job_Component::Update_Entity_Choose_New_Job()
{
	Clear_Job();
	
	active_job = service_locator->get_Scene_Graph()->Return_Job_With_Highest_Priority_Correlation(object_locator->Return_Object_Pointer());
}

void AI_Job_Component::Clear_Job()
{
	Clear_All_Goals_From_Array();
	active_job = NULL;
	current_num_job_goals = 0;
	next_goal_index = 0;
}

// Check for Messages Cycle

void AI_Job_Component::Check_For_Messages()
{
	switch (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type())
	{
	case OBJECT_TYPE_STRUCTURE:
		Check_For_Messages_Structure();
		break;
	case OBJECT_TYPE_ENTITY:
		Check_For_Messages_Entity();
		break;
	}
}

void AI_Job_Component::Check_For_Messages_Structure()
{
	Custom_Message* new_message;
	Coordinate current_coord;
	int job_type = 0;

	if (active_job != NULL) job_type = active_job->Return_Job_Type();

	switch (job_type)
	{
	case 1: // Oxygenate Job From job csv data
		for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
		{
			new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
			if (new_message->Read_Message(0) == MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION)	next_goal_index = 0;
		}
		break;
	case 2: // Door Job From job csv data
		for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
		{
			new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
			switch (new_message->Read_Message(0))
			{
			case MESSAGE_TYPE_SG_ENTITY_MOVEMENT:
				current_coord = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
				if (abs(new_message->Read_Message(6) - current_coord.x) <= 1 && abs(new_message->Read_Message(7) - current_coord.y) <= 1 && new_message->Read_Message(3) == object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION))
				{
					next_goal_index = 0;
				}
				break;
			}
		}
		break;
	}
}

void AI_Job_Component::Check_For_Messages_Entity()
{
	// STAGE 1:
	// Add all job messages to potential job array

	// Parse Job Messages

	// Send out Job Acceptance Message

	// Check to See if Someone Else Accepted the Job


	for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	{
		switch (service_locator->get_MB_Pointer()->Custom_Message_Array[i].Read_Message(0))
		{
		case MESSAGE_TYPE_ENTITY_JOB_REQUEST:
			break;
		}
	}
}


// Goal Addition and Subtraction 

bool AI_Job_Component::Is_Current_Job_Still_Relevant()
{
	if (active_job != NULL && active_job->Return_Is_Init() == true) return true;
	else return false;
}


bool AI_Job_Component::Query_Job_For_Goals()
{
	Goal_Set* goals_received_from_job = active_job->Return_Job_Goals(object_locator->Return_Object_Pointer());

	if (goals_received_from_job != NULL && goals_received_from_job->mini_goal_array.size() > 0)
	{
		int insertion_index = next_goal_index;
		for (int i = 0; i < goals_received_from_job->mini_goal_array.size(); i++)
		{
			current_goals.insert(current_goals.begin() + insertion_index, goals_received_from_job->mini_goal_array[i]);
			current_num_job_goals++;
			insertion_index++;
		}
		return true;
	}
	else
	{
		cout << "current job returned NULL goal set, should not happen" << endl;
	}

	return false;

}

void AI_Job_Component::Add_Non_Job_Goal_To_Goal_Array(Job_Goal goal)
{
	current_goals.push_back(goal);
	current_num_job_goals++;
}

void AI_Job_Component::Clear_Goal_From_Goal_Array(int array_position)
{
	current_goals.erase(current_goals.begin() + array_position);
}

void AI_Job_Component::Clear_All_Goals_From_Array()
{
	current_goals.clear();
}

// Goal Execution

void AI_Job_Component::Process_Next_Goal()
{
	// WE NEVER DELETE GOALS ONCE THEY HAVE BEEN ADDED

	if (next_goal_index < current_num_job_goals && current_goals[next_goal_index].goal_instruction_array[0] != ACTION_NONE)
	{
		//if (object_locator->Return_AI_Stats_Pointer()->Return_Object_Type() == OBJECT_TYPE_ENTITY) cout << current_goals[next_goal_index].goal_string_name << endl;
		Action_Triage(&current_goals[next_goal_index]);
	}
}

void AI_Job_Component::Action_Triage(Job_Goal* goal)
{
	
	switch (goal->goal_instruction_array[0])
	{
	case ACTION_CREATE:
		Action_Create(goal);
		break;
	case ACTION_DESTROY:
		Action_Destroy(goal);
		break;
	case ACTION_EDIT_INTERNAL:
		Action_Edit_Internal(goal);
		break;
	case ACTION_EDIT_EXTERNAL:
		Action_Edit_External(goal);
		break;
	case ACTION_ASSESS_INTERNAL:
		Action_Assess_Internal(goal);
		break;
	case ACTION_ASSESS_EXTERNAL:
		Action_Assess_External(goal);
		break;
	case ACTION_GOAL_TRAVERSAL:
		Action_Goal_Traversal(goal);
		break;
	case ACTION_CLOSE_OUT_GOAL_SET:
		Action_Manage_Goal_Set_End(goal);
		break;
	case ACTION_TRANSFER:
		Action_Transfer(goal);
		break;
	}
}

void AI_Job_Component::Action_Create(Job_Goal* goal)
{

}

void AI_Job_Component::Action_Destroy(Job_Goal* goal)
{

}

void AI_Job_Component::Action_Edit_Internal(Job_Goal* goal)
{
	switch (goal->goal_instruction_array[1])
	{
	case A_EI_PAUSE_FOR_FURTHER_INSTRUCTIONS:
		break;
	case A_EI_OPEN_DOOR:
		object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_INCREMENT_UNTIL_COMPLETE);
		wait_timer = goal->goal_instruction_array[2];
		next_goal_index++;
		break;
	case A_EI_WAIT:
		if (wait_timer <= 0) next_goal_index++;
		else wait_timer--;
		break;
	case A_EI_SET_WAIT_TIMER:
		wait_timer = goal->goal_instruction_array[2];
		next_goal_index++;
		break;
	case A_EI_CLOSE_DOOR:
		object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_DECREMENT_UNTIL_COMPLETE);
		next_goal_index++;
		break;
	case A_EI_SET_TARGET_COORD_TO_OBJECT:
		{
		Object* object = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3]);
		Coordinate destination = service_locator->get_Scene_Graph()->Return_Nearest_Accessible_Coordinate(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object->get_coordinate(), object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION));
		object_locator->Return_AI_Movement_Pointer()->Set_Target_Coord(destination);
		next_goal_index++;
		}
		break;
	case A_EI_STOP_MOVING:
		object_locator->Return_AI_Movement_Pointer()->Stop_Moving();
		next_goal_index++;
		break;
	}
}

void AI_Job_Component::Action_Edit_External(Job_Goal* goal)
{
	switch (goal->goal_instruction_array[1])
	{
	case A_EE_FIRE_MINING_LASER_AT_STRUCTURE:
		{int faction = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION);
		service_locator->get_Scene_Graph()->Create_Laser_Between_Two_Points(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Structure_By_Array_Num(goal->goal_instruction_array[2]), 2);
		service_locator->get_Scene_Graph()->Create_Projectile(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Structure_By_Array_Num(goal->goal_instruction_array[2]), 2, faction);
		next_goal_index++; }
		break;
	case A_EE_ADJUST_OBJECT_STAT:
		service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])->Return_Stats_Component()->Adjust_Stat(goal->goal_instruction_array[4], goal->goal_instruction_array[5]);
		next_goal_index++;
		break;
	}
}

void AI_Job_Component::Action_Assess_Internal(Job_Goal* goal)
{
	switch (goal->goal_instruction_array[1])
	{
	case A_AI_CHECK_JOB_FOR_MORE_GOALSETS:
		// We're going to move onto the next goal first, so that if we do add more goalsets, we're not repeating the "check job" goal later on
		next_goal_index++;
		if (active_job != NULL && active_job->Return_Num_Unassigned_Goalsets() > 0)
		{
			Query_Job_For_Goals();
		}

		break;
	}
}

void AI_Job_Component::Action_Assess_External(Job_Goal* goal)
{
	switch (goal->goal_instruction_array[1])
	{
	case A_AE_OXYGENATE:
		{
			vector<Coordinate> oxygenation_vector;
			map<Coordinate, bool> check_map;
			bool leak_check = false;
			service_locator->get_Scene_Graph()->Return_Tiles_Without_Leaks(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), oxygenation_vector, check_map, leak_check);
			if (leak_check == false && oxygenation_vector.size() > 0)
			{
				for (int i = 0; i < oxygenation_vector.size(); i++)
				{
					int message_content[8] = { MESSAGE_TYPE_STAT_UPDATE_REQUEST, OBJECT_TYPE_STRUCTURE, FOCUS_SPECIFC_OBJECT, STAT_ADJUST, oxygenation_vector[i].x,oxygenation_vector[i].y,STAT_STRUCTURE_OXYGEN_LEVEL,100 / oxygenation_vector.size() };
					service_locator->get_MB_Pointer()->Add_Custom_Message(8, message_content);
				}
				object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_INCREMENT_AND_REPEAT);
			}
			else
			{
				object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_PAUSE);
			}
			next_goal_index++;
			}
			break;
		case A_AE_CHECK_SIMPLE_DISTANCE:
			if (service_locator->get_Scene_Graph()->Check_Simple_Distance_To_Object(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])) <= goal->goal_instruction_array[4])
			{
				next_goal_index++;
		}
		break;
	case A_AE_CHECK_OBJECT_STAT:
		{AI_Stats_Component * stats_pointer = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[3], goal->goal_instruction_array[4])->Return_Stats_Component();
		switch (goal->goal_instruction_array[2])
		{
		case LOWER_THAN_OR_EQUAL_TO:
			{
				if (stats_pointer->Return_Stat_Value(goal->goal_instruction_array[5]) <= goal->goal_instruction_array[6]) next_goal_index += goal->goal_instruction_array[7];
				else next_goal_index++;
			}
			break;
		case HIGHER_THAN_OR_EQUAL_TO:
			{
				if (stats_pointer->Return_Stat_Value(goal->goal_instruction_array[5]) >= goal->goal_instruction_array[6]) next_goal_index += goal->goal_instruction_array[7];
				else next_goal_index++;
			}
			break;
		}}
		break;
	case A_AE_CHECK_FOR_ENTITIES_IN_RADIUS:
		{
			bool entities_in_radius = false;
			Coordinate base_coordinate = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
			int radius = goal->goal_instruction_array[2];
		
			if (service_locator->get_Scene_Graph()->Return_All_Entities_In_Radius(base_coordinate, goal->goal_instruction_array[2]).size() > 0)
			{
				entities_in_radius = true;
			}

			if (entities_in_radius == true)
			{
				next_goal_index += goal->goal_instruction_array[3];
			}
			else
			{
				next_goal_index++;
			}
		}
		break;
	case A_AE_CHECK_IF_OBJECT_HAS_ITEM:
		AI_Item_Component * object_inventory = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])->Return_AI_Item_Component();
		Item item;
		item.item_template_id = goal->goal_instruction_array[4];
		if (object_inventory->Return_Amount_Of_Item_In_Inventory(item) < goal->goal_instruction_array[5])
		{
			Abort_Current_Job();
		}
		else
		{
			next_goal_index++;
		}
		break;
	}
}

void AI_Job_Component::Action_Transfer(Job_Goal* goal)
{
	switch (goal->goal_instruction_array[1])
	{
	case A_AT_TAKE_ALL_INVENTORY_FROM_OBJECT:
		{AI_Item_Component* container_inventory = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])->Return_AI_Item_Component();
		object_locator->Return_AI_Item_Pointer()->Copy_Inventory_From_Pointer(container_inventory->Return_Entire_Inventory_As_Pointer(), container_inventory->Return_Num_Inventory_Slots());
		container_inventory->Clear_All_Inventory();
		}
		next_goal_index++;
		break;
	case A_AT_TAKE_ITEM_FROM_OBJECT:
		// WILL HAVE TO ADJUST THIS AT SOME POINT TO FIGURE OUT ITEM STATS.....
		object_locator->Return_AI_Item_Pointer()->Add_Item_To_Inventory(goal->goal_instruction_array[4], goal->goal_instruction_array[5], false, {});
		service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])->Return_AI_Item_Component()->Remove_Item_From_Inventory(goal->goal_instruction_array[4], goal->goal_instruction_array[5]);
		next_goal_index++;
		break;
	case A_AT_GIVE_ITEM_TO_OBJECT:
		// WILL HAVE TO ADJUST THIS AT SOME POINT TO FIGURE OUT ITEM STATS.....
		service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal->goal_instruction_array[2], goal->goal_instruction_array[3])->Return_AI_Item_Component()->Add_Item_To_Inventory(goal->goal_instruction_array[4], goal->goal_instruction_array[5], false, {});
		object_locator->Return_AI_Item_Pointer()->Remove_Item_From_Inventory(goal->goal_instruction_array[4], goal->goal_instruction_array[5]);
		next_goal_index++;
		break;
	}
}

void AI_Job_Component::Action_Goal_Traversal(Job_Goal* goal)
{
	next_goal_index += goal->goal_instruction_array[1];
}

void AI_Job_Component::Action_Manage_Goal_Set_End(Job_Goal* goal)
{
	active_job->Close_Out_Goal_Set(goal->goal_instruction_array[1], object_locator->Return_Object_Pointer());
	next_goal_index++;
}

void AI_Job_Component::Abort_Current_Job()
{
	cout << "aborting job" << endl;
	active_job->Abort_Job();
	Clear_Job();
}