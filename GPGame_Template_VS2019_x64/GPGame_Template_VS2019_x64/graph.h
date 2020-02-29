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

	void addNeighbour(const int neighbour, int cost = 0)
	{
		adjacent.emplace(neighbour,cost);
	}

	std::map<int, int> getConnections()
	{
		return adjacent;
	}

	int getID()
	{
		return id;
	}

	void setID(int i)
	{
		id = i;
	}

	void setDistance(const int d)
	{
		distance = d;
	}

	int getDistance() const
	{
		return distance;
	}

private:
	int id = -1;
	Position position;
	std::map<int, int> adjacent; //id, cost
	int distance = 0;
};

class Graph
{
public:
	Graph(){};
	~Graph(){};

	void addVertex(Vertex& new_vertex)
	{
		if (new_vertex.getID() == -1)
		{
			new_vertex.setID(num_vertices);
		}
		vert_dict.emplace(new_vertex.getID(), new_vertex);
		num_vertices++;
	}

	Vertex& getVertex(const int id)
	{
		auto it = vert_dict.find(id);
		return it->second;
	}

	void addEdge(const int frm, const int to, int cost = 0)
	{
		try
		{
			Vertex& v_from = vert_dict.find(frm)->second;
			Vertex& v_to = vert_dict.find(to)->second;
			v_from.addNeighbour(to, cost);
			v_to.addNeighbour(frm, cost);
		}
		catch(std::exception& e)
		{
			std::cout << e.what();
			//std::cout << "Attempted to add edge frm: " << frm << " to: " << to << "\n";
		}
	}

	std::map<int, Vertex> getVertices() const
	{
		return vert_dict;
	}

	int getNumber()
	{
		return num_vertices;
	}

	bool checkID(int id)
	{
		auto it = vert_dict.find(id);
		if (it == vert_dict.end())
			return false;
		else
			return true;
	}

private:
	std::map<int, Vertex> vert_dict;
	int num_vertices = 0;
};