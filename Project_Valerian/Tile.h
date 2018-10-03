#pragma once

#include<Object.h>
#include<SDL.h>
#include<Coordinate.h>
#include<Game_Library.h>

class Tile
{
public:
	Tile(Coordinate grid_point = { 0,0 });

	int Return_Tile_Type_By_Layer(int tile_layer);
	bool Update_Tile_Structure(Object* new_structure, int tile_layer);
	Object* Return_Tile_Object(int layer);

private:
	Object* base_structure = NULL;
	Object* mid_structure = NULL;
	Object* roof_structure = NULL;

	Coordinate grid_point;
};