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

	// Config for Renderer Component
	int render_component_type = RENDER_COMPONENT_NONE;
	int spritesheet = SPRITESHEET_NONE;
	SDL_Rect tile_specs = { 0,0,0,0 };
	SDL_Rect tile_clip = { 0,0,0,0 };
	int multiclip_type = MULTICLIP_NONE;
	int num_animation_frame = 0;

	// Config for Job
	int job_type = JOB_NONE;
};

struct Entity_Template
{
	int entity_id = ENTITY_ID_NULL;
	int render_component = RENDER_COMPONENT_ENTITY_COMPLEX;
	int num_entity_animations = 0;
	int num_entity_components = 0;
	Animation_State entity_animations[MAX_NUM_ANIMATIONS][MAX_NUM_COMPONENTS];
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

	void Load_Tiles_From_Data_File(string path);
	void Load_Entity_Templates(string entity_template_path, string entity_animation_path);
	vector<vector<int>> Create_Room_From_Data_File(int x_tile_start, int y_tile_start, string filename);
	bool is_wall(int tile_type);
	bool is_null(int tile_type);
	bool is_floor(int tile_type);




private:

	Structure_Template loaded_tiles[MAX_NUM_TILE_TEMPLATES];
	int num_loaded_tiles = 0;

	Entity_Template loaded_Entities[MAX_NUM_ENTITY_TEMPLATES];
	int num_loaded_entities = 0;

	vector<string> readCSVRow(const std::string &row);
	vector<vector<string>> readCSV(string path);
	int Query_Vector_Table(vector<vector<string>> table, int num_columns_to_crossrefernece, int column_1, int value_1, int column_2, int value_2, int value_column);

};

