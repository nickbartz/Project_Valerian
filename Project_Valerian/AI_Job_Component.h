#pragma once

#include<Service_Locator.h>
#include<Global_Constants.h>
#include<Game_Library.h>

struct Structure_Template;


class AI_Job_Component
{
// A class that runs jobs when and if an object has a job
public:
	AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Structure_Template sTemplate);
	AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, Entity_Template eTemplate);

	void Load_New_Job(int instruction_array_length, int job[]);
	void Check_For_Messages();
	void Update();

private:
	int job_id = 0;
	int structure_job_repeat = 0;

	int wait_timer = 0;

	int current_goals[MAX_NUM_GOAL_INSTRUCTIONS][MAX_LENGTH_GOAL_INSTRUCTION];
	int current_num_goals = 0;
	int next_goal_num = 0;

	void Clear_Instruction_Array();
	void Clear_Instruction(int array_position);
	void Add_Instruction_To_Array(int array_position, int instruction_size, int instruction[]);
	void Print_Instruction_From_Array(int array_position);
	void Print_Raw_Instruction(int instruction_size, int instruction[]);
	void Print_Goal_Array();

	void Process_Next_Goal();
	void Increment_Goal(int goal_increment);
	void Decrement_Goal(int goal_decrement);

	void Action_Triage(int goal[]);

	void Action_Create(int goal[]);
	void Action_Destroy(int goal[]);
	void Action_Edit_Internal(int goal[]);
	void Action_Edit_External(int goal[]);
	void Action_Assess_Internal(int goal[]);
	void Action_Assess_External(int goal[]);
	void Action_Transfer(int goal[]);
	void Action_Goal_Traversal(int goal[]);

	void Structure_Job_Triage(int job);

	void Structure_Job_Oxygenate_Surroundings();
	void Structure_Job_Open_Door();
	void Structure_Send_Out_Mining_Request();

	Global_Service_Locator * service_locator;
	Object_Service_Locator * object_locator;
	Structure_Template structure_template;
};