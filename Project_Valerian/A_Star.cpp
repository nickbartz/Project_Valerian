#include <A_Star.h>
#include<iostream>
#include<queue>
#include<Service_Locator.h>
#include<Scene_Graph.h>
#include<Coordinate.h>

Path_Field::Path_Field(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
}

vector<Tile_Queue> Path_Field::pathFind(Coordinate start, Coordinate end, int max_closed_nodes)
{
	int min_x = min(start.x, end.x);
	int min_y = min(start.y, end.y);
	
	int min_search_width = end.x - start.x;
	int min_search_height = end.y - start.y;
	
	int x_buffer = max(0,(SEARCH_WIDTH - min_search_width) / 2);
	int y_buffer = max(0,(SEARCH_HEIGHT - min_search_height) / 2);

	int world_offset_x = min_x - x_buffer;
	int world_offset_y = min_y - y_buffer;
	
	// USED FOR PATHFINDING 
	int obstacle_map[SEARCH_WIDTH][SEARCH_WIDTH];
	int closed_nodes_map[SEARCH_WIDTH][SEARCH_HEIGHT];
	int open_nodes_map[SEARCH_WIDTH][SEARCH_HEIGHT];
	int dir_map[SEARCH_WIDTH][SEARCH_HEIGHT];
	
	int start_x = start.x - world_offset_x;
	int start_y = start.y - world_offset_y;
	int finish_x = end.x - world_offset_x;
	int finish_y = end.y - world_offset_y;
	int node_check_count = 0;

	vector<Tile_Queue> vector_path;
	priority_queue<node> pq[2]; // list of open (not-yet-tried) nodes
	static int pqi; // pq index
	static node* n0;
	static node* m0;
	static int i, j, x, y, xdx, ydy;
	static char c;
	pqi = 0;

	// reset the node maps
	for (y = 0; y < SEARCH_HEIGHT; y++)
	{
		for (x = 0; x < SEARCH_WIDTH; x++)
		{
			obstacle_map[x][y] = 0;
			closed_nodes_map[x][y] = 0;
			open_nodes_map[x][y] = 0;
		}
	}

	// create the start node and push into list of open nodes
	n0 = new node(start_x, start_y, 0, 0);
	n0->updatePriority(finish_x, finish_y);
	pq[pqi].push(*n0);
	open_nodes_map[x][y] = n0->getPriority(); // mark it on the open nodes map

											  // A* search
	while (!pq[pqi].empty() && node_check_count <= max_closed_nodes)
	{
		node_check_count += 1;
		// get the current node w/ the highest priority
		// from the list of open nodes
		n0 = new node(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
			pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

		x = n0->getxPos(); y = n0->getyPos();

		pq[pqi].pop(); // remove the node from the open list
		open_nodes_map[x][y] = 0;
		// mark it on the closed nodes map
		closed_nodes_map[x][y] = 1;

		// quit searching when the goal state is reached
		//if((*n0).estimate(xFinish, yFinish) == 0)
		if (x == finish_x && y == finish_y)
		{
			// generate the path from finish to start
			// by following the directions
			int counter = 0;

			while (!(x == start_x && y == start_y))
			{
				j = dir_map[x][y];

				vector_path.push_back({ x + world_offset_x, y + world_offset_y,counter });
				x += dx[j];
				y += dy[j];
				counter++;
			}
			
			// garbage collection
			delete n0;
			// empty the leftover nodes
			while (!pq[pqi].empty()) pq[pqi].pop();
			return vector_path;

		}
		// generate moves (child nodes) in all possible directions
		for (i = 0; i<4; i++)
		{
			xdx = x + dx[i]; ydy = y + dy[i];


			if (!(xdx<0 || xdx>SEARCH_WIDTH - 1 || ydy<0 || ydy>SEARCH_HEIGHT - 1 || check_if_tile_is_inaccessible(xdx, ydy, finish_x, finish_y, world_offset_x, world_offset_y) || closed_nodes_map[xdx][ydy] == 1))
			{
				// generate a child node
				m0 = new node(xdx, ydy, n0->getLevel(),
					n0->getPriority());
				m0->nextLevel(i);
				m0->updatePriority(finish_x, finish_y);

				// if it is not in the open list then add into that
				if (open_nodes_map[xdx][ydy] == 0)
				{
					open_nodes_map[xdx][ydy] = m0->getPriority();
					pq[pqi].push(*m0);
					// mark its parent node direction
					dir_map[xdx][ydy] = (i + 4 / 2) % 4;
				}
				else if (open_nodes_map[xdx][ydy]>m0->getPriority())
				{
					// update the priority info
					open_nodes_map[xdx][ydy] = m0->getPriority();
					// update the parent direction info
					dir_map[xdx][ydy] = (i + 4 / 2) % 4;

					// replace the node
					// by emptying one pq to the other one
					// except the node to be replaced will be ignored
					// and the new node will be pushed in instead
					while (!(pq[pqi].top().getxPos() == xdx && pq[pqi].top().getyPos() == ydy))
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pq[pqi].pop(); // remove the wanted node

								   // empty the larger size pq to the smaller one
					if (pq[pqi].size()>pq[1 - pqi].size()) pqi = 1 - pqi;
					while (!pq[pqi].empty())
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pqi = 1 - pqi;
					pq[pqi].push(*m0); // add the better node instead
				}
				else delete m0; // garbage collection
			}
		}
		delete n0; // garbage collection
	}
	cout << "did not find path" << endl;
	return vector_path; // no route found
}

bool Path_Field::check_if_tile_is_inaccessible(int xdx, int ydy, int finish_x, int finish_y, int offset_x, int offset_y)
{
	bool inaccessible = false;
	if (!(xdx == finish_x && ydy == finish_y))
	{
		inaccessible = service_locator->get_Scene_Graph()->Tile_Is_Inaccessible({ xdx + offset_x,ydy + offset_y });
	}
	return inaccessible;
}

void Path_Field::run_A_star(Coordinate start, Coordinate end)
{
	cout << "Map Size (X,Y): " << SEARCH_WIDTH << "," << SEARCH_HEIGHT << endl;
	cout << "Start: " << start.x << "," << start.y << endl;
	cout << "Finish: " << end.x << "," << end.y << endl;

	vector<Tile_Queue> route = pathFind(start, end, 1000);

	for (int i = 0; i < route.size(); i++)
	{
		cout << route[i].x_tile << ", " << route[i].y_tile << endl;
	}

	return;
}

bool operator<(const node & a, const node & b)
{
	return a.getPriority() > b.getPriority();
}
