#pragma once
#include<Global_Constants.h>
#include<vector>
#include<iostream>
#include<string>
#include<Service_Locator.h>

using namespace std;

class Object;

struct Job_Goal
{
	int job_goal_id = 0;
	string goal_string_name = "";
	int goal_length = 0;
	int goal_instruction_array[MAX_LENGTH_GOAL_INSTRUCTION];
};

struct Goal_Set
{
	void Add_Job_Goal_to_Goal_Set(Job_Goal new_goal, int position);
	void Add_Raw_Job_Goal_to_Goalset(string goal_description, int array[], int num_instructions_in_array);

	int goal_set_uniq_id = rand() % 100;
	int num_goals_in_set = 0;
	Object* assigned_object;
	int max_num_assigned_objects;
	vector<Job_Goal> mini_goal_array ;
};

class Job
{
public:
	Job(Global_Service_Locator* service_locator = NULL, int job_type = 0, string job_string_name = "NULL_JOB");
	
	// Functions to Alter the Job
	void Add_Goal_Set(Goal_Set new_goal_set);

	// Functions to Access the Job's Contents
	Goal_Set* Get_Job_Goals(Object* object);
	string Return_Job_String_Name();
	void Print_Job_String_Name();
	int Get_Job_Type();
	int Get_Job_Uniq_ID();
	int Get_Num_Goal_Sets();

	// Functions relating to how the job is stored on the scene graph
	void Set_Job_Array_Num(int job_array_num);
	int Return_Job_Array_Num();
	bool Return_Is_Init();
	void Set_Init(bool new_init);

	void Close_Out_Goal_Set(int goal_set_uniq_id);

private:

	Global_Service_Locator * service_locator;

	int job_array_num = -1;
	bool init = false;
	string job_name;
	int uniq_id;
	
	int job_type;
	
	vector<Goal_Set> smart_goal_set;
	int current_num_goal_sets;

	Object* job_requestor;
	Object* attached_entities[MAX_ENTITIES_PER_JOB];

};