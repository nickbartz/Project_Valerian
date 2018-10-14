#include<Object.h>
#include<SDL.h>
#include<Global_Constants.h>
#include<AI_Stats_Component.h>
#include<Game_Library.h>
#include<Adjacent_Type_Array.h>
#include<iostream> 
#include<Message_Array.h>
#include<AI_Movement_Component.h>
#include<AI_Rel_Component.h>
#include<AI_Item_Component.h>
#include<Coordinate.h>
using namespace std;

Object::Object(int index, SDL_Rect location, Global_Service_Locator* sLocator)
{
	service_locator = sLocator;

	// set the array index of the object so we know where to find it in the scene graph object array later
	SG_object_array_index = index;

	uniq_id = rand() % 1000;

	temp_location = location;
}

void Object::Init_Structure_From_Template(Structure_Template object_config, Adjacent_Structure_Array neighbors)
{
	Structure_Stats structure_stats;
	structure_stats.impassable = object_config.is_inaccessible;

	object_service_locator.Register_Pointer(this);
	
	AI_Stats = new AI_Stats_Component(SG_object_array_index, service_locator, &object_service_locator, object_config.structure_id, structure_stats);
	object_service_locator.Register_Pointer(AI_Stats);

	render_component = new Render_Component(service_locator, &object_service_locator, object_config, neighbors);
	object_service_locator.Register_Pointer(render_component);

	AI_Job = new AI_Job_Component(service_locator, &object_service_locator, object_config);
	object_service_locator.Register_Pointer(AI_Job);

	AI_Movement = new AI_Movement_Component(service_locator, &object_service_locator, temp_location);
	object_service_locator.Register_Pointer(AI_Movement);

	AI_Relationship = new AI_Rel_Component(service_locator, &object_service_locator);
	object_service_locator.Register_Pointer(AI_Relationship);

	AI_Items = new AI_Item_Component(service_locator, &object_service_locator, 20, 0, 0, object_config.inventory_pack);
	object_service_locator.Register_Pointer(AI_Items);
}

void Object::Init_Entity_From_Template(Entity_Template object_config)
{	
	object_service_locator.Register_Pointer(this);

	AI_Stats = new AI_Stats_Component(SG_object_array_index, service_locator, &object_service_locator, OBJECT_TYPE_ENTITY, object_config.entity_id);
	object_service_locator.Register_Pointer(AI_Stats);

	render_component = new Render_Component(service_locator, &object_service_locator, object_config);
	object_service_locator.Register_Pointer(render_component);

	AI_Movement = new AI_Movement_Component(service_locator, &object_service_locator, temp_location);
	object_service_locator.Register_Pointer(AI_Movement);

	AI_Relationship = new AI_Rel_Component(service_locator, &object_service_locator);
	object_service_locator.Register_Pointer(AI_Relationship);

	AI_Items = new AI_Item_Component(service_locator, &object_service_locator, 20, 0, 0, object_config.entity_inventory_pack);
	object_service_locator.Register_Pointer(AI_Items);

	AI_Job = new AI_Job_Component(service_locator, &object_service_locator, object_config);
	object_service_locator.Register_Pointer(AI_Job);

}

void Object::Init_Projectile_From_Template(Projectile_Template projectile_config, SDL_Point target)
{
	object_service_locator.Register_Pointer(this);

	AI_Stats = new AI_Stats_Component(SG_object_array_index, service_locator, &object_service_locator, OBJECT_TYPE_PROJECTILE, projectile_config.projectile_template_id);
	object_service_locator.Register_Pointer(AI_Stats);

	render_component = new Render_Component(service_locator, &object_service_locator, OBJECT_TYPE_PROJECTILE, projectile_config.projectile_template_id);
	object_service_locator.Register_Pointer(render_component);

	AI_Movement = new AI_Movement_Component(service_locator, &object_service_locator, temp_location);
	AI_Movement->Set_Projectile_Velocity(target);
	object_service_locator.Register_Pointer(AI_Movement);
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

bool Object::Is_Structure_Inaccessible(int requesting_faction)
{
	if (service_locator->get_Game_Library()->is_door(AI_Stats->Get_Structure_Type()))
	{
		if (AI_Relationship->Return_Object_Faction() == requesting_faction) return false;
		else return true;
	}
	else
	{
		int inaccessible = AI_Stats->Return_Stat_Value(STAT_STRUCTURE_IMPASSABLE);
		if (inaccessible == 1) return true;
		else if (inaccessible == 0) return false;
	}
}

int Object::Return_Object_Array_Index()
{
	return SG_object_array_index;
}

Coordinate Object::get_coordinate()
{
	return AI_Movement->Return_Grid_Coord();
}

void* Object::Return_Object_Component_Pointer(int component_type)
{
	switch (component_type)
	{
	case OBJECT_COMPONENT_STATS:
		return AI_Stats;
		break;
	case OBJECT_COMPONENT_MOVEMENT:
		return AI_Movement;
		break;
	case OBJECT_COMPONENT_ITEM:
		return AI_Items;
		break;
	case OBJECT_COMPONENT_JOB:
		return AI_Job;
		break;
	case OBJECT_COMPONENT_RELATIONSHIP:
		return AI_Relationship;
		break;
	}
}

// Core Object Functions

void Object::Update()
{
	if (AI_Stats != NULL) AI_Stats->Update();
	if (AI_Job != NULL) AI_Job->Update();
	if (AI_Movement != NULL) AI_Movement->Update();
	if (AI_Relationship != NULL) AI_Relationship->Update();
	if (render_component != NULL) render_component->Update();
}

void Object::Collect_Bus_Messages()
{
	Coordinate grid_coord = AI_Movement->Return_Grid_Coord();
	if (render_component != NULL) render_component->Check_For_Messages(grid_coord.x,grid_coord.y);
	if (AI_Stats != NULL) AI_Stats->Check_For_Messages();
	if (AI_Job != NULL) AI_Job->Check_For_Messages();
	if (AI_Movement != NULL) AI_Movement->Check_For_Messages();
	if (AI_Relationship != NULL) AI_Relationship->Check_For_Messages();
}

void Object::Set_Assigned_Flag(int aFlag)
{
	assigned_flag = aFlag;
}

void Object::Draw()
{
	if(render_component != NULL) render_component->Draw(AI_Movement->Return_World_Pos());
}

void Object::Draw(SDL_Rect overwrite_pos)
{		
	if (render_component != NULL) render_component->Draw(overwrite_pos);
}

void Object::free()
{
	if (render_component != NULL) delete render_component, render_component = NULL;
	if (AI_Stats != NULL) delete AI_Stats, AI_Stats = NULL;
	if (AI_Job != NULL) delete AI_Job, AI_Job = NULL;
	if (AI_Movement != NULL) delete AI_Movement, AI_Movement = NULL;
	if (AI_Relationship != NULL) delete AI_Relationship, AI_Relationship = NULL;
	if (AI_Items != NULL) delete AI_Items, AI_Items = NULL;
}