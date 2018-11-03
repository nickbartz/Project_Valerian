#include<Job.h>

Job::Job(int jType, string job_string_name)
{
	job_name = job_string_name;
	job_type = jType;
}

void Job::Print_Job_String_Name()
{
	cout << job_name << endl;
}

void Job::Add_Job_Goal(Job_Goal new_goal, int position)
{
	goal_array[position] = new_goal;
}

int Job::Get_Num_Job_Goals()
{
	return total_job_goals;
}
Job_Goal Job::Get_Job_Goal(int goal_num)
{
	if (goal_num < total_job_goals)	return goal_array[goal_num];
	else
	{
		cout << "Requesting a job goal that is higher than max num goals, this should not happen" << endl;
		return Job_Goal{};
	}
}