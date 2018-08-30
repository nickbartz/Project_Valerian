#pragma once
#include<Global_Constants.h>
#include<Render_Component.h>
#include<AI_Component.h>

class SDL_Renderer;
class Service_Locator;
class Object_Config;
struct Adjacent_Structure_Array;

class Object
{
public:
	Object(int array_index =0, SDL_Rect location = { 0,0,0,0 }, Service_Locator* service_locator = NULL);

	int Get_Array_Index();
	int Get_Assigned_Flag();

	void Draw(SDL_Rect camera, SDL_Rect pos_rect);

	void Assign_Overlay_Renderer();
	void Assign_Simple_Clip_Tile_Renderer(Service_Locator* service_locator, Object_Config object_config);
	void Assign_Multi_Clip_Tile_Renderer(Service_Locator* service_locator, Object_Config object_config, Adjacent_Structure_Array neighbors );
	void Assign_Background_Renderer(Service_Locator* service_locator, Object_Config object_config);
	
	void Assign_Core_AI_Component(Service_Locator* service_locator, Object_Config object_config);
	void Assign_Basic_Structure_AI_Component(Service_Locator* service_locator, Object_Config object_config);

	// Unassigning an object means we're setting its type to unassigned so that it can be overwritten in whatever array it is in
	void Set_Assigned_Flag(int assigned_flag);

	void free();

private:
	Service_Locator* service_locator;

	int SG_object_array_index;

	// An unassigned object can be overwritten by a new object in an array
	int assigned_flag = OBJECT_UNASSIGNED;

	SDL_Rect object_pos;
	
	// Overlay Render Component
	Overlay_Renderer * overlay_renderer = NULL;

	// Optional Render Components
	Simple_Clip_Tile_Renderer * simple_tile_clip = NULL;
	Multi_Clip_Tile_Renderer * tile_multi_clip = NULL;
	Background_Renderer * background_clip = NULL;

	// Core AI Component
	Core_AI_Component* core_AI = NULL;
	Basic_Structure_AI* basic_structure_AI = NULL;

	// Physics Components


};

