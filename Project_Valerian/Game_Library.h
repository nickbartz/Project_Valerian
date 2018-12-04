#pragma once

using namespace std;
#include<SDL.h>
#include<Adjacent_Type_Array.h>
#include<Global_Constants.h>
#include<vector>
#include<SDL.h>
#include<Render_Component.h>
#include<Job.h>
#include<unordered_map>
#include<AI_Item_Component.h>

struct Blueprint;

struct Blueprint_Pack
{
	int blueprint_pack_id = 0;
	vector<int> blueprint_id_array;
};

struct Structure_Template
{
	int object_type = OBJECT_TYPE_NULL;;

	// Config for Structures AI
	int tile_layer = TILE_LAYER_BASE;
	int structure_type = STRUCTURE_TYPE_NULL;
	int structure_id = STRUCTURE_ID_NULL;
	string structure_name = "Null";
	string structure_type_string = "Null Type";

	// Config for Renderer Component
	int render_component_type = RENDER_COMPONENT_NONE;
	int spritesheet = SPRITESHEET_NONE;
	SDL_Rect tile_specs = { 0,0,0,0 };
	SDL_Rect tile_clip = { 0,0,0,0 };
	int multiclip_type = MULTICLIP_NONE;
	int multiclip_variant = 0;
	int num_animation_frame = 0;
	int ui_type = 0;
	int icon_clip_x = 0;
	int icon_clip_y = 0;

	// Config for Job
	int job_type = JOB_NULL;

	// Other stats
	int max_built_level = 255;
	int is_inaccessible = 0;

	// Item_Pack
	int has_starter_inventory = 0;
	int scaffold_blueprint_id = 0;
	int blueprint_pack_id = 0;
};

struct Entity_Template
{
	int entity_id = ENTITY_ID_NULL;
	int render_component = RENDER_COMPONENT_ENTITY_COMPLEX;
	int num_entity_animations = 0;
	int num_entity_components = 0;
	int entity_starter_inventory_blueprint = 0;
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
	int animation_delay = 0;
	SDL_Color projectile_color = { 0,0,0,0 }; // For instances where a projectile is a simple point render, or for particle effects
	int is_point_not_sprite = 0;
	int render = 1;
};

struct Item_Template
{
	int object_type = OBJECT_TYPE_ITEM;

	int inventory_item_id = 0;
	int inventory_item_type_id = 0;
	int associated_blueprint_id = 0;
	string inventory_item_name;
	string inventory_item_type_string;
	string inventory_item_description;
	int item_build_time = 0;

	SDL_Rect sprite_specs = { 0,0,SPRITE_SIZE,SPRITE_SIZE };
};

struct Equipment_Template
{

};

struct Name_Data
{
	vector<string> male_first;
	vector<string> male_middle;
	vector<string> male_end;
	vector<string> female_first;
	vector<string> female_middle;
	vector<string> female_end;
	vector<string> neutral_first;
	vector<string> neutral_middle;
	vector<string> neutral_end;

};

enum class CSVState {
	UnquotedField,
	QuotedField,
	QuotedQuote
};

class Game_Library
{
public:
	Game_Library(Global_Service_Locator* service_locator);

	// Acessors of Templates
	Structure_Template* Fetch_Structure_Template(int tile_id);
	Entity_Template* Fetch_Entity_Template(int entity_id);
	Item_Template* Fetch_Item_Template(int item_id);
	Blueprint* Fetch_Blueprint(int inventory_id);
	Blueprint_Pack* Fetch_Blueprint_Pack(int blueprint_pack);

	Job* Fetch_Job_Template(int job_id);
	Projectile_Template* Fetch_Projectile_Template(int projectile_id);
	string Fetch_Random_Name_Syllable(int entity_gender, int name_part);

	// Accessors of Template Info
	int Get_Num_Structure_Template();
	int Get_Structure_Type_Code_From_Structure_Type_String(string name);
	void Load_Job_Code_String_To_Enum_Crosswalk();
	int Get_Job_Code_From_String(string query);
	int Get_Object_Type_Enum_From_Object_Type_String(string query);
	int Get_Item_Type_Code_From_Item_Type_String(string item_type_string);

	void Load_Structures_From_Data_File(string tiles_path);
	void Load_Entity_Templates(string entity_template_path, string entity_animation_path);
	void Load_Item_Templates(string item_template_path);
	void Load_Blueprints(string blueprints_path);
	void Load_Blueprint_Packs(string blueprint_packs_path);
	void Load_Jobs(string jobs_path, string goals_path);
	void Load_Projectiles(string projectiles_path);
	void Load_Name_Data(string name_path);


	vector<vector<int>> Create_Room_From_Data_File(int x_tile_start, int y_tile_start, string filename);

	bool is_wall(int tile_type);
	bool is_null(int tile_type);
	bool is_floor(int tile_type);
	bool is_door(int tile_type);

private:
	Global_Service_Locator * service_locator;

	Structure_Template loaded_tiles[MAX_NUM_TEMPLATES];
	unordered_map<string, int> structure_type_codes;
	int num_loaded_tiles = 0;

	Entity_Template loaded_Entities[MAX_NUM_TEMPLATES];
	int num_loaded_entities = 0;

	Item_Template loaded_Items[MAX_NUM_TEMPLATES];
	unordered_map<string, int> item_type_codes;
	int num_loaded_Items = 0;

	Blueprint loaded_blueprints[MAX_NUM_TEMPLATES];
	int num_loaded_blueprints = 0;

	Blueprint_Pack loaded_blueprint_packs[MAX_NUM_TEMPLATES];
	int num_loaded_blueprint_packs = 0;

	unordered_map<string, int> job_codes;


	Job loaded_jobs[MAX_NUM_TEMPLATES];
	int num_loaded_jobs = 0;

	Projectile_Template loaded_projectiles[MAX_NUM_TEMPLATES];
	int num_loaded_projectiles = 0;

	Name_Data name_database;

	vector<string> readCSVRow(const std::string &row);
	vector<vector<string>> readCSV(string path);
	int Query_Vector_Table(vector<vector<string>> table, int num_columns_to_crossrefernece, int column_1, int value_1, int column_2, int value_2, int value_column);



};

