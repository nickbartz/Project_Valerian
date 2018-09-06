#include<Object.h>
#include<SDL.h>
#include<Global_Constants.h>
#include<AI_Component.h>
#include<Game_Library.h>
#include<Adjacent_Type_Array.h>
#include<iostream> 
using namespace std;

Object::Object(int index, SDL_Rect location, Service_Locator* sLocator)
{
	service_locator = sLocator;
	object_pos = location;

	// set the array index of the object so we know where to find it in the scene graph object array later
	SG_object_array_index = index;


	uniq_id = rand() % 1000;
}

void Object::Init_From_Object_Config(Object_Config object_config, Adjacent_Structure_Array neighbors)
{
	Assign_AI_Components(object_config);
	Assign_Physics_Components(object_config);
	Assign_Render_Components(object_config, neighbors);
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
	if (basic_structure_AI != NULL)
	{
		basic_structure_AI->Get_Structure_Type();
	}
	else
	{
		return STRUCTURE_TYPE_NULL;
	}
}

// Individual Component Assignment Functions
void Object::Assign_Overlay_Renderer()
{
	overlay_renderer = new Overlay_Renderer(service_locator);
}

void Object::Assign_Simple_Clip_Tile_Renderer(Service_Locator* service_locator, Object_Config object_config )
{
	simple_tile_clip = new Simple_Clip_Tile_Renderer(object_config.tile_specs, service_locator, object_config.spritesheet, object_config.tile_clip);
}

void Object::Assign_Background_Renderer(Service_Locator* service_locator, Object_Config object_config)
{
	background_clip = new Background_Renderer(service_locator, object_config.spritesheet, object_config.tile_clip);
}

void Object::Assign_Multi_Clip_Tile_Renderer(Service_Locator* service_locator, Object_Config object_config, Adjacent_Structure_Array neighbors)
{
	tile_multi_clip = new Multisprite_Tile_Renderer(service_locator, object_config.tile_specs, object_config.spritesheet , object_config.multiclip_type, neighbors);
}

void Object::Assign_Core_AI_Component(Service_Locator* service_locator, Object_Config object_config)
{
	core_AI = new Core_AI_Component(object_config.object_type);
}

void Object::Assign_Basic_Structure_AI_Component(Service_Locator* service_locator, Object_Config object_config)
{
	basic_structure_AI = new Basic_Structure_AI(object_config.structure_type, object_config.structure_id);
}

// Macro Component Assignment Functions

void Object::Assign_AI_Components(Object_Config object_config)
{
	Assign_Core_AI_Component(service_locator, object_config);

	if (object_config.object_type == OBJECT_TYPE_STRUCTURE)
	{
		Assign_Basic_Structure_AI_Component(service_locator, object_config);
	}
}

void Object::Assign_Physics_Components(Object_Config object_config)
{

}

void Object::Assign_Render_Components(Object_Config object_config, Adjacent_Structure_Array neighbors)
{

	Assign_Overlay_Renderer();

	switch (object_config.render_component_type)
	{
	case RENDER_COMPONENT_BACKGROUND:
		Assign_Background_Renderer(service_locator, object_config);
		break;
	case RENDER_COMPONENT_SIMPLECLIP:
		Assign_Simple_Clip_Tile_Renderer(service_locator, object_config);
		break;
	case RENDER_COMPONENT_MULTICLIP:
		Assign_Multi_Clip_Tile_Renderer(service_locator, object_config, neighbors);
		break;
	}
}


// Core Object Functions

void Object::Set_Assigned_Flag(int aFlag)
{
	assigned_flag = aFlag;
}

void Object::Draw(SDL_Rect camera, SDL_Rect overwrite_pos)
{	
	if (overlay_renderer != NULL) overlay_renderer->Draw();
	if (simple_tile_clip != NULL) simple_tile_clip->Draw(object_pos);
	if (background_clip != NULL) background_clip->Draw(overwrite_pos);
	if (tile_multi_clip != NULL) tile_multi_clip->Draw(object_pos);
}

void Object::free()
{
	if (overlay_renderer != NULL) delete overlay_renderer, overlay_renderer = NULL;
	if (simple_tile_clip != NULL) delete simple_tile_clip, simple_tile_clip = NULL;
	if (background_clip != NULL) delete background_clip, background_clip = NULL;
	if (tile_multi_clip != NULL) delete tile_multi_clip, tile_multi_clip = NULL;

	if (core_AI != NULL) delete core_AI, core_AI = NULL;
	if (basic_structure_AI != NULL) delete basic_structure_AI, basic_structure_AI = NULL;
}