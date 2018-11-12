#include<Job.h>
#include<Scene_Graph.h>

void Goal_Set::Add_Job_Goal_to_Goal_Set(Job_Goal new_goal, int position)
{
	mini_goal_array.push_back(new_goal);
	num_goals_in_set++;
}

void Goal_Set::Add_Raw_Job_Goal_to_Goalset(string goal_description, int array[], int num_instructions_in_array)
{
	Job_Goal new_goal;
	
	new_goal.goal_string_name = goal_description;

	for (int i = 0; i < num_instructions_in_array; i++)
	{
		new_goal.goal_instruction_array[i] = array[i];
	}

	new_goal.goal_length = num_instructions_in_array;

	mini_goal_array.push_back(new_goal);
	num_goals_in_set++;
}

Job::Job(Global_Service_Locator* sLocator, int jType, string job_string_name)
{
	service_locator = sLocator;
	job_name = job_string_name;
	job_type = jType;
	uniq_id = rand() % 1000;
}

void Job::Print_Job_String_Name()
{
	cout << job_name << endl;
}

string Job::Return_Job_String_Name()
{
	return job_name;
}

int Job::Get_Job_Uniq_ID()
{
	return uniq_id;
}

int Job::Get_Num_Goal_Sets()
{
	return current_num_goal_sets;
}

int Job::Get_Num_Unassigned_Goalsets()
{
	int num_unassigned_goalsets = 0;
	for (int i = 0; i < current_num_goal_sets; i++)
	{
		if (smart_goal_set[i].current_num_assigned_objects < smart_goal_set[i].max_num_assigned_objects)
		{
			num_unassigned_goalsets++;
		}
	}

	return num_unassigned_goalsets;
}

void Job::Add_Goal_Set(Goal_Set new_goal_set)
{
	smart_goal_set.push_back(new_goal_set);
	current_num_goal_sets++;
}

Goal_Set* Job::Get_Job_Goals(Object* object)
{	
	for (int i = 0; i < smart_goal_set.size(); i++)
	{
		if (smart_goal_set[i].current_num_assigned_objects < smart_goal_set[i].max_num_assigned_objects)
		{
			smart_goal_set[i].assigned_objects.push_back(object);
			smart_goal_set[i].current_num_assigned_objects++;
			return &smart_goal_set[i];
		}
	}

	cout << "returning a null goal_set" << endl;
	return NULL;
}

int Job::Get_Job_Type()
{
	return job_type;
}

bool Job::Return_Is_Init()
{
	return init;
}

void Job::Set_Init(bool new_init)
{
	init = new_init;
}

void Job::Set_Job_Array_Num(int j_array_num)
{
	job_array_num = j_array_num;
}

int Job::Return_Job_Array_Num()
{
	return job_array_num;
}

void Job::Close_Out_Goal_Set(int goal_set_id)
{
	for (int i = 0; i < current_num_goal_sets; i++)
	{
		if (smart_goal_set[i].goal_set_uniq_id == goal_set_id)
		{
			smart_goal_set.erase(smart_goal_set.begin() + i);
			current_num_goal_sets--;
			break;
		}
	}

	if (current_num_goal_sets <= 0)
	{
		service_locator->get_Scene_Graph()->Check_If_Job_Can_Be_Closed(job_array_num);
	}
}