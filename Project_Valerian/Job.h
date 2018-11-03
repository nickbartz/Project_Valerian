#pragma once
#include<Global_Constants.h>
#include<vector>
#include<iostream>
#include<string>

using namespace std;

class Object;

struct Job_Goal
{
	int job_goal_id = 0;
	string goal_string_name = "";
	int goal_length = 0;
	int goal_instruction_array[MAX_LENGTH_GOAL_INSTRUCTION];
};

class Job
{
public:
	Job(int job_type = 0, string job_string_name = "NULL_JOB");
	
	void Add_Job_Goal(Job_Goal new_goal, int position);
	int Get_Num_Job_Goals();
	Job_Goal Get_Job_Goal(int goal_num);
	void Print_Job_String_Name();

private:
	string job_name;
	
	int job_type;
	
	int total_job_goals = 0;
	Job_Goal goal_array[MAX_NUM_GOAL_INSTRUCTIONS];

	Object* job_requestor;
	Object* attached_entities[MAX_ENTITIES_PER_JOB];

};