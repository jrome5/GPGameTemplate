#include "Astar.h"
#include "graph.h"

constexpr int max_number = 1000;

int heuristic(Vertex a, Vertex b)
{
    auto manhattan_vertical = abs(b.getPosition().y - a.getPosition().y);
    auto manhattan_horizontal = abs(b.getPosition().x - a.getPosition().x);
    return manhattan_horizontal + manhattan_vertical;
}

/*
 * Extract the path from the traversal graph
 */
vector<Vertex> reconstruct_path(Graph g, int start_id, int goal_id, map<int, int>& came_from)
{
    vector<Vertex> path;
    Vertex current = g.getVertex(goal_id);
    path.push_back(current);
    int current_id = current.getID();
    while (current_id != start_id)
    {
        current_id = came_from[current_id];
        current = g.getVertex(current_id);
        path.push_back(current);
        auto pos = current.getPosition();
        //std::cout << pos.x << ", " << pos.y << "\n";
    }

    path.push_back(g.getVertex(start_id)); // optional
    std::reverse(path.begin(), path.end());

    return path;
}

class CompareDist
{
public:
    bool operator()(pair<Vertex, int> n1, pair<Vertex, int> n2) {
        return n1.second > n2.second;
    }
};

/*
 * An implementation of the A* algorithm
 */
void a_star_search(const Graph& g, const int start_id, const int goal_id, vector<Vertex>& path)
{
    Graph graph = g;
    Vertex goal = graph.getVertex(goal_id);
    Vertex start = graph.getVertex(start_id);
    map<int, int> came_from; // id, id
    map<int, int> cost_so_far; // vertex id, cost
    std::priority_queue<pair<Vertex, int>, vector<pair<Vertex, int>>, CompareDist> frontier;
    //PriorityQueue frontier;
    std::pair<Vertex, int> s(start, 0);
    start.setDistance(0);
    frontier.push(s);

    came_from[start_id] = NULL;
    cost_so_far[start_id] = 0;

    while (!frontier.empty()) {
        Vertex current = frontier.top().first;
        frontier.pop();
        int current_id = current.getID();
        if (current_id == goal_id)
            break;
        
        for (auto pair : current.getConnections()) //int id, int cost pair
        {
            int next_id = pair.first;
            Vertex& next = graph.getVertex(next_id);
            int new_cost = cost_so_far[current_id] + pair.second;
            if (!cost_so_far.count(next_id) || new_cost < cost_so_far.find(next_id)->second)
            {
                cost_so_far[next_id] = new_cost;
                auto priority = new_cost + heuristic(next, goal);
                next.setDistance(priority);
                std::pair<Vertex, int> p(next, priority);
                frontier.push(p);
                came_from[next_id] = current_id;
            }
        }
    }

    path = reconstruct_path(graph, start_id, goal_id, came_from);
}

