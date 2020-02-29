#pragma once

#include <map>
#include <set>
#include <array>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include "graph.h"

using std::map;
using std::vector;
using std::pair;

vector<Vertex> reconstruct_path(Graph g, int start_id, int goal_id, map<int, int>& came_from);
int heuristic(Vertex a, Vertex b);

void a_star_search(const Graph& g, const int start_id, const int goal_id, vector<Vertex>& path);
