#pragma once

#include <iostream>
using namespace std;

struct Coordinate
{
	int x;
	int y;

	bool operator<(const Coordinate& coord) const {
		if (x < coord.x) return true;
		if (x > coord.x) return false;
		//x == coord.x
		if (y < coord.y) return true;
		if (y > coord.y) return false;
		//x == coord.x && y == coord.y
		//*this == coord
		return false;
	}

	void print()
	{
		cout << x << ", " << y << endl;
	}
};
