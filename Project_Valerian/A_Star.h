#pragma once

#include<vector>
#include<algorithm>
#include<Global_Constants.h>

class Global_Service_Locator;
class Coordinate;

using namespace std;

struct Tile_Queue
{
	int x_tile = 0;
	int y_tile = 0;
	int order = 0;
};

class Path_Field {
public:
	Path_Field(Global_Service_Locator* service_locator);
	vector<Tile_Queue> pathFind(Coordinate start, Coordinate end, int max_closed_nodes);
	void run_A_star(Coordinate start, Coordinate end);
	bool check_if_tile_is_inaccessible(int xdx, int ydy, int finish_x, int finish_y, int offset_x, int offset_y);

private:
	Global_Service_Locator * service_locator;

	const int dx[4] = { 1, 0, -1, 0 };
	const int dy[4] = { 0, 1, 0, -1 };

};

class node
{
	// current position
	int xPos;
	int yPos;
	// total distance already travelled to reach the node
	int level;
	// priority=level+remaining distance estimate
	int priority;  // smaller: higher priority

public:
	node(int xp, int yp, int d, int p)
	{
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	// Determine priority (in the priority queue)

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest)
	{
	priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// give better priority to going strait instead of diagonally
	void nextLevel(const int & i) // i: direction
	{
		level += 10;
	}

	// Estimation function for the remaining distance to the goal.
	const int & estimate(const int & xDest, const int & yDest) const
	{
		static int xd, yd, d;
		xd = xDest - xPos;
		yd = yDest - yPos;

		// Euclidian Distance
		//d = static_cast<int>(sqrt(xd*xd + yd*yd));

		// Manhattan distance
		//d=abs(xd)+abs(yd);

		// Chebyshev distance
		d = max(abs(xd), abs(yd));

		return(d);
	}
};
