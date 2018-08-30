#pragma once

#include<Object.h>
#include<SDL.h>
#include<Coordinate.h>

class Tile
{
public:
	Tile(Coordinate grid_point = { 0,0 });

	bool Update_Tile_Structure(Object* new_structure, int tile_layer);

private:
	Object * base_structure = NULL;
	Object* mid_structure = NULL;
	Object* roof_structure = NULL;

	Coordinate grid_point;
};