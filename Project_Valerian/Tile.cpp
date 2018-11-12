#include<Tile.h>
#include <iostream>
#include<Object.h>
using namespace std;

Tile::Tile(Coordinate gPoint)
{
	grid_point = gPoint;
}

bool Tile::Update_Tile_Structure(Object* new_structure, int tile_layer)
{
	bool replacement = false;
	
	switch (tile_layer)
	{
	case TILE_LAYER_BASE:
		if (base_structure != NULL && base_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			base_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
			replacement = true;
		}
		base_structure = new_structure;
		break;
	case TILE_LAYER_MID:
		if (mid_structure != NULL && mid_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			mid_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
			replacement = true;
		}
		mid_structure = new_structure;
		break;
	case TILE_LAYER_ROOF:
		if (roof_structure != NULL && roof_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			replacement = true;
			roof_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
		}
		roof_structure = new_structure;
		break;
	case TILE_LAYER_SCAFFOLD:
		if (scaffold != NULL && scaffold->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			replacement = true;
			scaffold->Set_Assigned_Flag(OBJECT_UNASSIGNED);
		}
		scaffold = new_structure;
		break;
	}

	return replacement;
}

int Tile::Return_Tile_Type_By_Layer(int tile_layer)
{
	if (tile_layer == TILE_LAYER_SCAFFOLD && scaffold != NULL) return scaffold->Get_Structure_Type();
	else if (tile_layer == TILE_LAYER_BASE && base_structure != NULL) return base_structure->Get_Structure_Type();
	else if (tile_layer == TILE_LAYER_MID && mid_structure != NULL) return mid_structure->Get_Structure_Type();
	else if (tile_layer == TILE_LAYER_ROOF && roof_structure != NULL) return roof_structure->Get_Structure_Type();
	else return STRUCTURE_TYPE_NULL;
}

Object* Tile::Return_Tile_Object(int layer)
{
	switch (layer)
	{
	case TILE_LAYER_SCAFFOLD:
		return scaffold;
		break;
	case TILE_LAYER_BASE:
		return base_structure;
		break;
	case TILE_LAYER_MID:
		return mid_structure;
		break;
	case TILE_LAYER_ROOF:
		return roof_structure;
		break;
	}
}