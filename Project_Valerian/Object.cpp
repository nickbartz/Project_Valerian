#include<Object.h>
#include<SDL.h>
#include<Global_Constants.h>
#include<AI_Stats_Component.h>
#include<Game_Library.h>
#include<Adjacent_Type_Array.h>
#include<iostream> 
#include<Message_Array.h>
using namespace std;

Object::Object(int index, SDL_Rect location, Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
	object_pos = location;
	grid_x = object_pos.x / TILE_SIZE;
	grid_y = object_pos.y / TILE_SIZE;

	// set the array index of the object so we know where to find it in the scene graph object array later
	SG_object_array_index = index;

	uniq_id = rand() % 1000;
}

void Object::Init_Structure_From_Template(Structure_Template object_config, Adjacent_Structure_Array neighbors)
{
	Structure_Stats structure_stats;
	structure_stats.grid_x = grid_x;
	structure_stats.grid_y = grid_y;
	AI_Stats = new AI_Stats_Component(service_locator, &object_service_locator, object_config, structure_stats);
	object_service_locator.Register_Pointer(AI_Stats);

	render_component = new Render_Component(service_locator, &object_service_locator, object_config, neighbors);
	object_service_locator.Register_Pointer(render_component);

	AI_Job = new AI_Job_Component(service_locator, &object_service_locator, object_config);
}

void Object::Init_Entity_From_Template(Entity_Template object_config)
{	
	AI_Stats = new AI_Stats_Component(service_locator, &object_service_locator, object_config);
	object_service_locator.Register_Pointer(AI_Stats);

	render_component = new Render_Component(service_locator, &object_service_locator, object_config);
	object_service_locator.Register_Pointer(render_component);
}

int Object::Get_Assigned_Flag()
{
	return assigned_flag;
}

int Object::Get_Array_Index()
{
	return SG_object_array_index;
}

int Object::Get_Structure_Type()
{
	if (AI_Stats != NULL)
	{
		AI_Stats->Get_Structure_Type();
	}
	else
	{
		return STRUCTURE_TYPE_NULL;
	}
}



// Core Object Functions

void Object::Update()
{
	if (AI_Stats != NULL) AI_Stats->Update();
	if (AI_Job != NULL) AI_Job->Update();
}

void Object::Collect_Bus_Messages()
{
	if (render_component != NULL) render_component->Check_For_Messages(grid_x,grid_y);
	if (AI_Stats != NULL) AI_Stats->Check_For_Messages();
	if (AI_Job != NULL) AI_Job->Check_For_Messages();
}

void Object::Set_Assigned_Flag(int aFlag)
{
	assigned_flag = aFlag;
}

void Object::Draw()
{
	render_component->Draw(object_pos);
}

void Object::Draw(SDL_Rect overwrite_pos)
{		
	render_component->Draw(overwrite_pos);
}

void Object::free()
{
	if (render_component != NULL) delete render_component, render_component = NULL;
	if (AI_Stats != NULL) delete AI_Stats, AI_Stats = NULL;
}