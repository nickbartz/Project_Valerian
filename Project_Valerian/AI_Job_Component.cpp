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

AI_Job_Component::AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate)
{
	service_locator = sLocator;
	object_locator = oLocator;
	structure_template = sTemplate;
	job_id = structure_template.job_type;

	Clear_Instruction_Array();
	
	if (structure_template.job_type != 0)
	{
		Structure_Job_Triage(structure_template.job_type);
	}
}

AI_Job_Component::AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate)
{
	service_locator = sLocator;
	object_locator = oLocator;
	job_id = 0;

	Clear_Instruction_Array();
}

void AI_Job_Component::Check_For_Messages()
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
				if (new_message->Read_Message(6) == current_coord.x && new_message->Read_Message(7) == current_coord.y && new_message->Read_Message(3) == object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION))
				{
					Structure_Job_Open_Door();
				}
				break;
			}
		}
		break;

	}
}

void AI_Job_Component::Update()
{
	if (structure_job_repeat == 1) Structure_Job_Triage(job_id);
	
	Process_Next_Goal();
}

void AI_Job_Component::Clear_Instruction_Array()
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

void AI_Job_Component::Clear_Instruction(int array_num)
{
	for (int i = 0; i < MAX_LENGTH_GOAL_INSTRUCTION; i++)
	{
		current_goals[array_num][i] = 0;
	}
}

void AI_Job_Component::Add_Instruction_To_Array(int array_position, int instruction_size, int instruction[])
{
	Clear_Instruction(array_position);

	for (int i = 0; i < instruction_size; i++)
	{
		current_goals[array_position][i] = instruction[i];
	}

	if (current_goals[array_position][instruction_size-1] != JOB_INST_END_GOAL)
	{
		cout << "Cannot Add Goal -- Incorrect Goal Format" << endl;
		Print_Raw_Instruction(instruction_size, instruction);
		Clear_Instruction(array_position);
	}
	else
	{
		current_num_goals++;
	}
}

void AI_Job_Component::Print_Instruction_From_Array(int array_position)
{
	for (int i = 0; i < MAX_LENGTH_GOAL_INSTRUCTION; i++)
	{
		cout << current_goals[array_position][i] << ", ";
	}

	cout << endl;
}

void AI_Job_Component::Print_Raw_Instruction(int instruction_length, int instruction[])
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
		Print_Instruction_From_Array(i);
	}
}

void AI_Job_Component::Process_Next_Goal()
{
	if (next_goal_num < MAX_NUM_GOAL_INSTRUCTIONS && current_goals[next_goal_num][0] != ACTION_NONE)
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
	case A_EI_SET_TARGET_COORD_TO_STRUCTURE:
		{
		Object* structure = service_locator->get_Scene_Graph()->Return_Structure_By_Array_Num(goal[1]);
		Coordinate destination = service_locator->get_Scene_Graph()->Return_Nearest_Accessible_Coordinate(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), structure->get_coordinate(), object_locator->Return_AI_Stats_Pointer()->Return_Stat_Value(STAT_OBJECT_FACTION));
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
	int check_for_leaks[3] = {ACTION_ASSESS_EXTERNAL, A_AE_OXYGENATE, JOB_INST_END_GOAL};
	Add_Instruction_To_Array(0, 3, check_for_leaks);
}

void AI_Job_Component::Structure_Job_Open_Door()
{
	Clear_Instruction_Array();
	next_goal_num = 0;
	
	int open_door[4] = { ACTION_EDIT_INTERNAL, A_EI_OPEN_DOOR, 20,JOB_INST_END_GOAL }; // final number here is the amount to wait before closing the door
	Add_Instruction_To_Array(0, 4, open_door);

	int wait[3] = { ACTION_EDIT_INTERNAL, A_EI_WAIT, JOB_INST_END_GOAL };
	Add_Instruction_To_Array(1, 3, wait);

	int close_door[3] = { ACTION_EDIT_INTERNAL, A_EI_CLOSE_DOOR, JOB_INST_END_GOAL };
	Add_Instruction_To_Array(2, 3, close_door);
}

void AI_Job_Component::Structure_Send_Out_Mining_Request()
{
	int message[42] = { 
		MESSAGE_TYPE_ENTITY_JOB_REQUEST, 
		JOB_TYPE_MINING, JOB_INST_START, 6,  // NUMBER OF GOALS IN JOB
		JOB_INST_NEW_GOAL, 4,  ACTION_EDIT_INTERNAL, A_EI_SET_TARGET_COORD_TO_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(), JOB_INST_END_GOAL, 
		JOB_INST_NEW_GOAL, 6,  ACTION_ASSESS_EXTERNAL, A_AE_CHECK_SIMPLE_DISTANCE, OBJECT_TYPE_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(),1, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_EDIT_EXTERNAL, A_EE_FIRE_MINING_LASER_AT_STRUCTURE, object_locator->Return_Object_Pointer()->Get_Array_Index(), JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_EDIT_INTERNAL, A_EI_SET_WAIT_TIMER, 20, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 3,  ACTION_EDIT_INTERNAL, A_EI_WAIT, JOB_INST_END_GOAL,
		JOB_INST_NEW_GOAL, 4,  ACTION_GOAL_TRAVERSAL, TRAVERSAL_GOAL_DECREMENT, 3, JOB_INST_END_GOAL,
		JOB_INST_END};

	service_locator->get_MB_Pointer()->Add_Custom_Message(42, message);

}

void AI_Job_Component::Load_New_Job(int job_array_length, int job[])
{
	Clear_Instruction_Array();
	
	job_id = job[0];
	int instruction_num = 0;
	int max_num_instructions = -1;

	for (int i = 1; i < job_array_length; i++)
	{
		if (job[i] == JOB_INST_START) max_num_instructions = job[i + 1];
		
		if (job[i] == JOB_INST_NEW_GOAL)
		{
			Add_Instruction_To_Array(instruction_num,job[i+1],&job[i+2]);
			instruction_num++;
		}
	}

	if (instruction_num != max_num_instructions)
	{
		Clear_Instruction_Array();
		cout << "Could Not Load - Job Corrupted" << endl;
	}
}

