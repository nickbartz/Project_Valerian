#include<AI_Job_Component.h>
#include<iostream>
#include<Coordinate.h>
#include<map>
#include<Scene_Graph.h>
#include<AI_Stats_Component.h>
#include<Message_Array.h>

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
		service_locator->get_Scene_Graph()->Return_Tiles_Without_Leaks(object_locator->Return_AI_Stats_Pointer()->Get_Structure_Coordinate(), oxygenation_vector, check_map, leak_check);
		if (leak_check == false && oxygenation_vector.size() > 0)
		{
			for (int i = 0; i < oxygenation_vector.size(); i++)
			{
				int message_content[7] = { MESSAGE_TYPE_STAT_UPDATE_REQUEST, OBJECT_TYPE_STRUCTURE, FOCUS_SPECIFC_OBJECT, oxygenation_vector[i].x,oxygenation_vector[i].y,STAT_STRUCTURE_OXYGEN_LEVEL,10 };
				service_locator->get_MB_Pointer()->Add_Custom_Message(7, message_content);
			}
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