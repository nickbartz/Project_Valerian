#pragma once

#include<Service_Locator.h>
#include<Global_Constants.h>
#include<Game_Library.h>

struct Structure_Template;

class AI_Job_Component
{
public:
	AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate);
	void Check_For_Messages();
	void Update();


private:
	int current_goals[MAX_LENGTH_GOAL_INSTRUCTION][MAX_NUM_GOAL_INSTRUCTION];
	int next_goal_num = 0;

	void Clear_Instruction_Array();
	void Add_Instruction_To_Array(int array_position, int instruction_size, int instruction[]);

	void Process_Next_Goal();

	void Action_Triage(int goal[]);
	void Action_Create(int goal[]);
	void Action_Destroy(int goal[]);
	void Action_Edit_Internal(int goal[]);
	void Action_Edit_External(int goal[]);
	void Action_Assess_Internal(int goal[]);
	void Action_Assess_External(int goal[]);
	void Action_Transfer(int goal[]);

	void Job_Triage(int job);
	void Job_Oxygenate_Surroundings();
	
	Global_Service_Locator * service_locator;
	Object_Service_Locator * object_locator;
	Structure_Template structure_template;
};