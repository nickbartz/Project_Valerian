#include<AI_Job_Component.h>
#include<iostream>
#include<Coordinate.h>
#include<map>
#include<Scene_Graph.h>
#include<AI_Stats_Component.h>
#include<Message_Array.h>
#include<AI_Movement_Component.h>
#include<AI_Rel_Component.h>

AI_Job_Component::AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate)
{
	service_locator = sLocator;
	object_locator = oLocator;
	structure_template = sTemplate;

	Clear_Instruction_Array();
	if (structure_template.job_type != 0) Job_Triage(structure_template.job_type);
}

void AI_Job_Component::Check_For_Messages()
{
	Custom_Message* new_message;
	Coordinate current_coord; 

	switch (structure_template.job_type)
	{
	case JOB_OXYGENATE_SURROUNDINGS:
		for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
		{
			new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
			if (new_message->Read_Message(0) == MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION)
			{
				next_goal_num = 0;
			}
		}
		break;
	case JOB_DOOR_STANDARD:
		for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
		{
			new_message = &service_locator->get_MB_Pointer()->Custom_Message_Array[i];
			switch (new_message->Read_Message(0))
			{
			case MESSAGE_TYPE_SG_ENTITY_MOVEMENT:
				current_coord = object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord();
				if (new_message->Read_Message(6) == current_coord.x && new_message->Read_Message(7) == current_coord.y && new_message->Read_Message(3) == object_locator->Return_AI_Rel_Pointer()->Return_Object_Faction())
				{
					Job_Open_Door();
				}
				break;
			}
		}
		break;

	}
}

void AI_Job_Component::Update()
{
	Process_Next_Goal();
}

void AI_Job_Component::Clear_Instruction_Array()
{
	for (int i = 0; i < MAX_LENGTH_GOAL_INSTRUCTION; i++)
	{
		for (int p = 0; p < MAX_NUM_GOAL_INSTRUCTION; p++)
		{
			current_goals[i][p] = 0;
		}
	}

}

void AI_Job_Component::Add_Instruction_To_Array(int array_position, int instruction_size, int instruction[])
{
	for (int i = 0; i < instruction_size; i++)
	{
		current_goals[array_position][i] = instruction[i];
	}
}

void AI_Job_Component::Process_Next_Goal()
{
	if (next_goal_num < MAX_NUM_GOAL_INSTRUCTION && current_goals[next_goal_num][0] != ACTION_NONE)
	{
		Action_Triage(current_goals[next_goal_num]);
	}
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
		object_locator->Return_Render_Pointer()->Change_Structure_Current_Animation(STRUCTURE_ANIMATION_INCREMENT_UNTIL_COMPLETE);
		wait_timer = goal[1];
		next_goal_num++;
		break;
	case A_EI_WAIT:
		if (wait_timer <= 0) next_goal_num++;
		else wait_timer--;
		break;
	case A_EI_CLOSE_DOOR:
		object_locator->Return_Render_Pointer()->Change_Structure_Current_Animation(STRUCTURE_ANIMATION_DECREMENT_UNTIL_COMPLETE);
		break;
	}
}
void AI_Job_Component::Action_Edit_External(int goal[])
{

}
void AI_Job_Component::Action_Assess_Internal(int goal[])
{

}
void AI_Job_Component::Action_Assess_External(int goal[])
{
	switch (goal[0])
	{
	case A_AE_OXYGENATE:
		vector<Coordinate> oxygenation_vector;
		map<Coordinate, bool> check_map;
		bool leak_check = false;
		service_locator->get_Scene_Graph()->Return_Tiles_Without_Leaks(object_locator->Return_AI_Movement_Pointer()->Return_Grid_Coord(), oxygenation_vector, check_map, leak_check);
		if (leak_check == false && oxygenation_vector.size() > 0)
		{
			for (int i = 0; i < oxygenation_vector.size(); i++)
			{
				int message_content[8] = { MESSAGE_TYPE_STAT_UPDATE_REQUEST, OBJECT_TYPE_STRUCTURE, FOCUS_SPECIFC_OBJECT, STAT_ADJUST, oxygenation_vector[i].x,oxygenation_vector[i].y,STAT_STRUCTURE_OXYGEN_LEVEL,100/oxygenation_vector.size() };
				service_locator->get_MB_Pointer()->Add_Custom_Message(8, message_content);
			}
			object_locator->Return_Render_Pointer()->Change_Structure_Current_Animation(STRUCTURE_ANIMATION_INCREMENT_REPEAT);
		}
		else
		{
			object_locator->Return_Render_Pointer()->Change_Structure_Current_Animation(STRUCTURE_ANIMATION_PAUSE);
		}
		next_goal_num++;
		break;
	}
}
void AI_Job_Component::Action_Transfer(int goal[])
{

}

void AI_Job_Component::Job_Triage(int job)
{
	switch (job)
	{
	case JOB_OXYGENATE_SURROUNDINGS:
		Job_Oxygenate_Surroundings();
		break;
	}
}

void AI_Job_Component::Job_Oxygenate_Surroundings()
{
	int check_for_leaks[3] = {ACTION_ASSESS_EXTERNAL, A_AE_OXYGENATE};
	Add_Instruction_To_Array(0, 2, check_for_leaks);
}

void AI_Job_Component::Job_Open_Door()
{
	Clear_Instruction_Array();
	next_goal_num = 0;
	
	int open_door[3] = { ACTION_EDIT_INTERNAL, A_EI_OPEN_DOOR, 20 }; // final number here is the amount to wait before closing the door
	Add_Instruction_To_Array(0, 3, open_door);

	int wait[2] = { ACTION_EDIT_INTERNAL, A_EI_WAIT };
	Add_Instruction_To_Array(1, 2, wait);

	int close_door[2] = { ACTION_EDIT_INTERNAL, A_EI_CLOSE_DOOR };
	Add_Instruction_To_Array(2, 2, close_door);
}