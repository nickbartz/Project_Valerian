#pragma once

using namespace std;
#include<SDL.h>
#include<Adjacent_Type_Array.h>
#include<Global_Constants.h>
#include<vector>
#include<SDL.h>
#include<Render_Component.h>

struct Structure_Template
{
	int object_type = OBJECT_TYPE_NULL;;

	// Config for Structures AI
	int tile_layer = TILE_LAYER_BASE;
	int structure_type = STRUCTURE_TYPE_NULL;
	int structure_id = STRUCTURE_ID_NULL;
	string structure_name = "Null";

	// Config for Renderer Component
	int render_component_type = RENDER_COMPONENT_NONE;
	int spritesheet = SPRITESHEET_NONE;
	SDL_Rect tile_specs = { 0,0,0,0 };
	SDL_Rect tile_clip = { 0,0,0,0 };
	int multiclip_type = MULTICLIP_NONE;
	int num_animation_frame = 0;
	int ui_type = 0;
	int icon_clip_x = 0;
	int icon_clip_y = 0;

	// Config for Job
	int job_type = JOB_NULL;

	// Other stats
	int is_inaccessible = 0;

	// Inventory_Pack
	int inventory_pack = 0;
};

struct Entity_Template
{
	int entity_id = ENTITY_ID_NULL;
	int render_component = RENDER_COMPONENT_ENTITY_COMPLEX;
	int num_entity_animations = 0;
	int num_entity_components = 0;
	int entity_inventory_pack = 0;
	Animation_State entity_animations[MAX_NUM_ANIMATIONS][MAX_NUM_COMPONENTS];
};

struct Projectile_Template
{
	int projectile_template_id = 0;
	int projectile_power = 0;
	int projectile_speed = 0;
	int projectile_range = 0;
	int projectile_splash = 0;
	string projectile_string;
	SDL_Rect sprite_clip;
	int num_animation_frames = 0;
};

struct Item_Template
{
	int object_type = OBJECT_TYPE_ITEM;

	int inventory_item_id = 0;
	int inventory_item_type = 0;

	SDL_Rect sprite_specs = { 0,0,SPRITE_SIZE,SPRITE_SIZE };
};

struct Inventory_Template
{
	int inventory_pack_num = 0;
	int inventory_pack[5][2];
};

struct Equipment_Template
{

};

struct Job_Template
{
	int job_id;
	string job_string_name;
	int job_standard_priority;
};

enum class CSVState {
	UnquotedField,
	QuotedField,
	QuotedQuote
};

class Game_Library
{
public:
	Game_Library();

	Structure_Template Fetch_Tile_Object_Config(int tile_id);
	Entity_Template Fetch_Entity_Template(int entity_id);
	Item_Template Fetch_Item_Template(int item_id);
	Inventory_Template Fetch_Blueprint(int inventory_id);
	Job_Template Fetch_Job_Template(int job_id);
	Projectile_Template Fetch_Projectile_Template(int projectile_id);

	int Get_Num_Structure_Template();

	void Load_Tiles_From_Data_File(string tiles_path);
	void Load_Entity_Templates(string entity_template_path, string entity_animation_path);
	void Load_Item_Templates(string item_template_path);
	void Load_Blueprints(string blueprints_path);
	void Load_Jobs(string jobs_path);
	void Load_Projectiles(string projectiles_path);

	vector<vector<int>> Create_Room_From_Data_File(int x_tile_start, int y_tile_start, string filename);
		
	bool is_wall(int tile_type);
	bool is_null(int tile_type);
	bool is_floor(int tile_type);
	bool is_door(int tile_type);

private:

	Structure_Template loaded_tiles[MAX_NUM_TEMPLATES];
	int num_loaded_tiles = 0;

	Entity_Template loaded_Entities[MAX_NUM_TEMPLATES];
	int num_loaded_entities = 0;

	Item_Template loaded_Items[MAX_NUM_TEMPLATES];
	int num_loaded_Items = 0;

	Inventory_Template loaded_blueprints[MAX_NUM_TEMPLATES];
	int num_loaded_blueprints = 0;

	Job_Template loaded_jobs[MAX_NUM_TEMPLATES];
	int num_loaded_jobs = 0;

	Projectile_Template loaded_projectiles[MAX_NUM_TEMPLATES];
	int num_loaded_projectiles = 0;

	vector<string> readCSVRow(const std::string &row);
	vector<vector<string>> readCSV(string path);
	int Query_Vector_Table(vector<vector<string>> table, int num_columns_to_crossrefernece, int column_1, int value_1, int column_2, int value_2, int value_column);

};

