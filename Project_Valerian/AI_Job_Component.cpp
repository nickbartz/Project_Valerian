#include<AI_Job_Component.h>
#include<iostream>
#include<Coordinate.h>
#include<map>
#include<Scene_Graph.h>
#include<AI_Stats_Component.h>
#include<Message_Array.h>
#include<AI_Movement_Component.h>
#include<AI_Rel_Component.h>
#include<Service_Locator.h>
#include<Cursor.h>
#include<Job.h>

// Instantiation

AI_Job_Component::AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int object_type, int template_id)
{
	service_locator = sLocator;
	object_locator = oLocator;

	Clear_Goal_Array();

	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		job_id = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(template_id)->job_type;
		current_job = service_locator->get_Game_Library()->Fetch_Job_Template(job_id);
		if (job_id != 0)
		{
			current_job->Print_Job_String_Name();
			Structure_Job_Triage(job_id);
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	case OBJECT_TYPE_CONTAINER:
		break;
	}
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
	if (structure_job_repeat == 1) Structure_Job_Triage(job_id);
	if (current_num_goals > 0) Process_Next_Goal();
}

void AI_Job_Component::Update_Entity()
{
	// STAGE 1: 
	// Work through potential jobs, remove any that are already taken

	// STAGE 2: 
	// Choose the job that is the highest priority

	// STAGE 3: 
	// Check to see if there are any local jobs that take precedence 

	// STAGE 4: 
	// a) If local-job takes precedence, process local job
	// b) If non-local job is allowed, process non-local job

	// STAGE 5: 
	// Send out message informing others that job is accepted 
	
	
	if (current_num_goals > 0) Process_Next_Goal();
}

void AI_Job_Component::Structure_Import_Job(int job_id)
{
	
}

void AI_Job_Component::Structure_Job_Triage(int job)
{
	switch (job)
	{
	case 1: // From Job Data in csv
		Structure_Job_Oxygenate_Surroundings();
		break;
	case 3: // From Job Data in csv
		Structure_Send_Out_Mining_Request();
		break;
	}
}

void AI_Job_Component::Structure_Job_Oxygenate_Surroundings()
{
	int check_for_leaks[3] = { ACTION_ASSESS_EXTERNAL, A_AE_OXYGENATE, JOB_INST_END_GOAL };
	Add_Goal_To_Array(0, 3, check_for_leaks);
}

void AI_Job_Component::Structure_Job_Open_Door()
{
	Clear_Goal_Array();
	next_goal_num = 0;

	int open_door[4] = { ACTION_EDIT_INTERNAL, A_EI_OPEN_DOOR, 20,JOB_INST_END_GOAL }; // final number here is the amount to wait before closing the door
	Add_Goal_To_Array(0, 4, open_door);

	int wait[3] = { ACTION_EDIT_INTERNAL, A_EI_WAIT, JOB_INST_END_GOAL };
	Add_Goal_To_Array(1, 3, wait);

	int close_door[3] = { ACTION_EDIT_INTERNAL, A_EI_CLOSE_DOOR, JOB_INST_END_GOAL };
	Add_Goal_To_Array(2, 3, close_door);
}

void AI_Job_Component::Structure_Send_Out_Mining_Request()
{
	int message[54] = {
		MESSAGE_TYPE_ENTITY_JOB_REQUEST,
		JOB_TYPE_MINING, JOB_INST_START, 7,  // NUMBER OF GOALS IN JOB
		JOB_INST_NEW_GOAL, 5,  ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(), JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 6,  ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(),1, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_EDIT_EXTERNAL, A_EE_FIRE_MINING_LASER_AT_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(), JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 9,  ACTION_ASSESS_EXTERNAL, A_AE_CHECK_OBJECT_STAT, LOWER_THAN_OR_EQUAL_TO, OBJECT_TYPE_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(), STAT_OBJECT_HEALTH, 0, 4, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, 0, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 3,  ACTION_EDIT_INTERNAL, A_EI_WAIT, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_GOAL_TRAVERSAL, TRAVERSAL_GOAL_DECREMENT, 4, JOB_INST_END_GOAL,
		JOB_INST_END };

	service_locator->get_MB_Pointer()->Add_Custom_Message(54, message);

}

void AI_Job_Component::Container_Send_Out_Pickup_Request()
{
	//int message[12] = {
	//	MESSAGE_TYPE_ENTITY_JOB_REQUEST,
	//	JOB_TYPE_CONTAINER_PICKUP, JOB_INST_START, 1,  // NUMBER OF GOALS IN JOB
	//	JOB_INST_NEW_GOAL, 5,  ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_OBJECT, OBJECT_TYPE_CONTAINER, object_locator->Return_Object_Pointer()->Get_Array_Index(), JOB_INST_END_GOAL,
	//	JOB_INST_END };

	//service_locator->get_MB_Pointer()->Add_Custom_Message(12, message);
}

void AI_Job_Component::Load_New_Job(int job_array_length, int job[])
{
	Clear_Goal_Array();

	job_id = job[0];
	int instruction_num = 0;
	int max_num_instructions = -1;

	for (int i = 1; i < job_array_length; i++)
	{
		if (job[i] == JOB_INST_START) max_num_instructions = job[i + 1];

		if (job[i] == JOB_INST_NEW_GOAL)
		{
			Add_Goal_To_Array(instruction_num, job[i + 1], &job[i + 2]);
			instruction_num++;
		}
	}

	if (instruction_num != max_num_instructions)
	{
		Clear_Goal_Array();
		cout << "Could Not Load - Job Corrupted" << endl;
	}
	else
	{
		next_goal_num = 0;
	}
}

void AI_Job_Component::Clear_Job()
{
	Clear_Goal_Array();
	job_id = 0;
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
	// THIS WILL PRIMARILY SERVE TO UPDATE PERMANENT STRUCTURAL JOBS RATHER THAN ENTITY JOBS
	Custom_Message* new_message;
	Coordinate current_coord;

	switch (job_id)
	{
	case 1: // Oxygenate Job From job csv data
		for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
		{
			new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
			if (new_message->Read_Message(0) == MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION)
			{
				next_goal_num = 0;
			}
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
					Structure_Job_Open_Door();
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

// LOWER LEVEL JOB MANAGEMENT FUNCTIONS 

// New functionality

void AI_Job_Component::Add_All_Job_Goals_To_Array()
{
	Clear_All_Goals_From_Array();

	for (int i = 0; i < current_job->Get_Num_Job_Goals(); i++)
	{
		goal_array[i] = current_job->Get_Job_Goal(i);
	}

}

void AI_Job_Component::Add_Job_Goal_To_Goal_Array(Job_Goal goal, int array_position)
{
	goal_array[array_position] = goal;
}

void AI_Job_Component::Clear_Goal_From_Goal_Array(int array_position)
{
	Job_Goal null_goal;
	goal_array[array_position] = null_goal;
}

void AI_Job_Component::Clear_All_Goals_From_Array()
{
	for (int i = 0; i < MAX_NUM_GOAL_INSTRUCTIONS; i++)
	{
		Clear_Goal_From_Goal_Array(i);
	}
}

// Job and Goal Creation

void AI_Job_Component::Clear_Goal_Array()
{
	for (int i = 0; i < MAX_NUM_GOAL_INSTRUCTIONS; i++)
	{
		for (int p = 0; p < MAX_LENGTH_GOAL_INSTRUCTION; p++)
		{
			current_goals[i][p] = 0;
		}
	}

	current_num_goals = 0;
}

void AI_Job_Component::Clear_Goal(int array_num)
{
	for (int i = 0; i < MAX_LENGTH_GOAL_INSTRUCTION; i++)
	{
		current_goals[array_num][i] = 0;
	}
}

void AI_Job_Component::Add_Goal_To_Array(int array_position, int instruction_size, int instruction[])
{
	Clear_Goal(array_position);

	for (int i = 0; i < instruction_size; i++)
	{
		current_goals[array_position][i] = instruction[i];
	}

	if (current_goals[array_position][instruction_size-1] != JOB_INST_END_GOAL)
	{
		cout << "Cannot Add Goal -- Incorrect Goal Format" << endl;
		Print_Raw_Goal(instruction_size, instruction);
		Clear_Goal(array_position);
	}
	else
	{
		current_num_goals++;
	}
}

void AI_Job_Component::Print_Goal_From_Array(int array_position)
{
	for (int i = 0; i < MAX_LENGTH_GOAL_INSTRUCTION; i++)
	{
		cout << current_goals[array_position][i] << ", ";
	}

	cout << endl;
}

void AI_Job_Component::Print_Raw_Goal(int instruction_length, int instruction[])
{
	for (int i = 0; i < instruction_length; i++)
	{
		cout << instruction[i] << ", ";
	}

	cout << endl;
}

void AI_Job_Component::Print_Goal_Array()
{
	for (int i = 0; i < current_num_goals; i++)
	{
		Print_Goal_From_Array(i);
	}
}

// Goal Execution

void AI_Job_Component::Process_Next_Goal()
{
	if (next_goal_num < current_num_goals && current_goals[next_goal_num][0] != ACTION_NONE)
	{
		Action_Triage(current_goals[next_goal_num]);
	}
}

void AI_Job_Component::Increment_Goal(int goal_increment)
{
	next_goal_num += goal_increment;
}

void AI_Job_Component::Decrement_Goal(int goal_increment)
{
	next_goal_num -= goal_increment;

	if (next_goal_num < 0) next_goal_num = 0;
}

void AI_Job_Component::Action_Triage(int goal[])
{
	switch (goal[0])
	{
	case ACTION_CREATE:
		Action_Create(&goal[1]);
		break;
	case ACTION_DESTROY:
		Action_Destroy(&goal[1]);
		break;
	case ACTION_EDIT_INTERNAL:
		Action_Edit_Internal(&goal[1]);
		break;
	case ACTION_EDIT_EXTERNAL:
		Action_Edit_External(&goal[1]);
		break;
	case ACTION_ASSESS_INTERNAL:
		Action_Assess_Internal(&goal[1]);
		break;
	case ACTION_ASSESS_EXTERNAL:
		Action_Assess_External(&goal[1]);
		break;
	case ACTION_GOAL_TRAVERSAL:
		Action_Goal_Traversal(&goal[1]);
	}
}

void AI_Job_Component::Action_Create(int goal[])
{

}

void AI_Job_Component::Action_Destroy(int goal[])
{

}

void AI_Job_Component::Action_Edit_Internal(int goal[])
{
	switch (goal[0])
	{
	case A_EI_OPEN_DOOR:
		object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_INCREMENT_UNTIL_COMPLETE);
		wait_timer = goal[1];
		next_goal_num++;
		break;
	case A_EI_WAIT:
		if (wait_timer <= 0) next_goal_num++;
		else wait_timer--;
		break;
	case A_EI_SET_WAIT_TIMER:
		wait_timer = goal[1];
		next_goal_num++;
		break;
	case A_EI_CLOSE_DOOR:
		object_locator->Return_Render_Pointer()->Change_Simple_Animation(SIMPLE_ANIMATION_DECREMENT_UNTIL_COMPLETE);
		next_goal_num++;
		break;
	case A_EI_SET_TARGET_COORD_TO_OBJECT:
		{
		Object* object = service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal[1], goal[2]);
		Coordinate destination = service_locator->get_Scene_Graph()->Return_Nearest_Accessible_Coordinate(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), object->get_coordinate(), object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION));
		object_locator->Return_AI_Movement_Pointer()->Set_Target_Coord(destination);
		next_goal_num++;
		}
		break;
	}
}

void AI_Job_Component::Action_Edit_External(int goal[])
{
	switch (goal[0])
	{
	case A_EE_FIRE_MINING_LASER_AT_STRUCTURE:
		int faction = object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION);
		service_locator->get_Scene_Graph()->Create_Laser_Between_Two_Points(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Structure_By_Array_Num(goal[1]), 2);
		service_locator->get_Scene_Graph()->Create_Projectile(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Structure_By_Array_Num(goal[1]), 2,faction);
		next_goal_num++;
		break;
	}
}

void AI_Job_Component::Action_Assess_Internal(int goal[])
{

}

void AI_Job_Component::Action_Assess_External(int goal[])
{
	switch (goal[0])
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
		next_goal_num++;
		}
		break;
	case A_AE_CHECK_SIMPLE_DISTANCE:
		if (service_locator->get_Scene_Graph()->Check_Simple_Distance_To_Object(object_locator->Return_Object_Pointer(), service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal[1], goal[2])) <= 1)
		{
			next_goal_num++;
		}
		break;
	case A_AE_CHECK_OBJECT_STAT:
		switch (goal[1])
		{
		case LOWER_THAN_OR_EQUAL_TO:
			{
			AI_Stats_Component * stats_pointer = (AI_Stats_Component*)service_locator->get_Scene_Graph()->Return_Object_By_Type_And_Array_Num(goal[2], goal[3])->Return_Object_Component_Pointer(OBJECT_COMPONENT_STATS);
			if (stats_pointer->Return_Stat_Value(goal[4]) <= goal[5]) next_goal_num += goal[6];
			else next_goal_num++;
			}
			break;
		}
		break;
	}
}

void AI_Job_Component::Action_Transfer(int goal[])
{

}

void AI_Job_Component::Action_Goal_Traversal(int goal[])
{
	switch (goal[0])
	{
	case TRAVERSAL_GOAL_INCREMENT:
		Increment_Goal(goal[1]);
		break;
	case TRAVERSAL_GOAL_DECREMENT:
		Decrement_Goal(goal[1]);
		break;
	}
}


