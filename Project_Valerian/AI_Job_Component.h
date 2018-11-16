#pragma once

#include<Service_Locator.h>
#include<Global_Constants.h>
#include<Game_Library.h>

struct Structure_Template;
class Job;


class AI_Job_Component
{
// A class that runs jobs when and if an object has a job
public:
	AI_Job_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator, int object_type, int template_id);

	void Update();

	void Check_For_Messages();

	Job* Return_Current_Job_Pointer();
	int Return_Current_Num_Goals();
	Job_Goal* Return_Goal_At_Array_Num(int array_num);
	int Return_Current_Goal_Index();

	// Abort Clears the Job and lets the job know that it is broken
	void Abort_Current_Job();

	// Clear just clears the job for the Entity
	void Clear_Job();
	void free();

private:
	Job* active_job;

	int wait_timer = 0;

	// Main Functions
	void Check_For_Messages_Structure();
	void Check_For_Messages_Entity();
	void Update_Structure();
	void Update_Entity();
	void Update_Entity_Choose_New_Job();

	// Job Functions
	void Update_Entity_Manage_Jobs();

	// Goal Functions
	vector<Job_Goal> current_goals;
	int current_num_job_goals = 0;
	int next_goal_index = 0;
	bool Is_Current_Job_Still_Relevant();
	bool Query_Job_For_Goals();
	void Add_Non_Job_Goal_To_Goal_Array(Job_Goal goal);
	void Clear_Goal_From_Goal_Array(int array_position);
	void Clear_All_Goals_From_Array();

	// Goal Instructions Processing Functions
	void Process_Next_Goal();

	void Action_Triage(Job_Goal* goal);

	void Action_Create(Job_Goal* goal);
	void Action_Destroy(Job_Goal* goal);
	void Action_Edit_Internal(Job_Goal* goal);
	void Action_Edit_External(Job_Goal* goal);
	void Action_Assess_Internal(Job_Goal* goal);
	void Action_Assess_External(Job_Goal* goal);
	void Action_Transfer(Job_Goal* goal);
	void Action_Goal_Traversal(Job_Goal* goal);
	void Action_Manage_Goal_Set_End(Job_Goal* goal);

	Global_Service_Locator * service_locator;
	Object_Service_Locator * object_locator;
};