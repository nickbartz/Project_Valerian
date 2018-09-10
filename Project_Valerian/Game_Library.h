#pragma once

using namespace std;
#include<SDL.h>
#include<Adjacent_Type_Array.h>
#include<Global_Constants.h>
#include<vector>


struct Object_Config
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

	Object_Config Fetch_Tile_Object_Config(int tile_id);
	void Load_Tiles_From_Data_File(string path);
	vector<vector<int>> Create_Room_From_Data_File(int x_tile_start, int y_tile_start, string filename);
	bool is_wall(int tile_type);
	bool is_null(int tile_type);
	bool is_floor(int tile_type);

private:

	Object_Config loaded_tiles[MAX_NUM_TILE_TEMPLATES];
	int num_loaded_tiles = 0;

	vector<string> readCSVRow(const std::string &row);
	vector<vector<string>> readCSV(string path);

};

