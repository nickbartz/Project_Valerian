#pragma once

class AI_Component
{
public:
	AI_Component();
private:

};


class Core_AI_Component :public AI_Component
{
public:
	Core_AI_Component(int oType) :AI_Component()
	{
		object_type = oType;
	}

private:
	int object_type;

};

class Basic_Structure_AI :public AI_Component
{
public:
	Basic_Structure_AI(int sType, int sName) :AI_Component()
	{
		structure_type = sType;
		structure_name = sName;
	}

	int Get_Structure_Name();
	int Get_Structure_Type();

private:
	int structure_type;
	int structure_name;

};