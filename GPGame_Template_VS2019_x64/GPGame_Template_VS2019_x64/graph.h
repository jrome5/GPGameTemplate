#pragma once
#include <map>
#include <exception>
typedef struct Position
{
	int x;
	int y;
} Position;

class Vertex
{
public:
	Vertex(){};
	~Vertex(){};

	void setPosition(const int pos_x, const int pos_y)
	{
		position = Position{ pos_x, pos_y };
	}

	Position getPosition()
	{
		return position;
	}

	void addNeighbour(const Vertex& neighbour, int cost = 0)
	{
		adjacent.emplace(cost, neighbour);
	}

	std::map<int, Vertex> getConnections()
	{
		return adjacent;
	}

	int getID()
	{
		return id;
	}

	void setWall(const bool wall)
	{
		is_wall = wall;
	}

private:
	int id;
	Position position;
	std::map<int, Vertex> adjacent;
	bool is_wall;
};

class Graph
{
public:
	Graph(){};
	~Graph(){};

	void addVertex(const Vertex& new_vertex)
	{
		vert_dict.emplace(num_vertices, new_vertex);
		num_vertices++;
	}

	Vertex getVertex(const int id)
	{
		auto it = vert_dict.find(id);
		if(it != vert_dict.end())
		{
			return it->second;
		}
	}

	void addEdge(const int frm, const int to, int cost = 0)
	{
		try
		{
			vert_dict[frm].addNeighbour(vert_dict[to], cost);
			vert_dict[to].addNeighbour(vert_dict[frm], cost);
		}
		catch(std::exception& e)
		{
			cout << "Attempted to add edge frm: " << frm << " to: " << to << "\n";
		}
	}

private:
	std::map<int, Vertex> vert_dict;
	int num_vertices = 0;
};