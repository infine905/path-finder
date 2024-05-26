#include "compute.h"

std::pair<float, std::vector<int>> find_path(const std::vector<Line>& lines, int start, int end)
{
    // Some staff for init graph and verteces
    std::set<int> vertices;
    std::unordered_map<int, std::vector<Edge>> graph;

    for (int i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        graph[line.start_idx].push_back({ line.end_idx, line.mass, i });
        vertices.insert(line.start_idx);
        vertices.insert(line.end_idx);
    }

    std::unordered_map<int, float> dist; //array for all distances
    std::unordered_map<int, int> prev_line; //array for previous lines
    std::unordered_map<int, int> prev_vertex; //array for previous verteces
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> pq; //priority queue for verteces by dist

    // verteces init
    for (int vertex : vertices) {
        dist[vertex] = std::numeric_limits<float>::infinity();
    }
    dist[start] = 0.0;

    pq.push({ 0.0, start });

    while (!pq.empty()) {
        float current_distance = pq.top().first;
        int current_vertex = pq.top().second;
        pq.pop();

        //If current vertex is end vertex return
        if (current_vertex == end) {
            std::vector<int> path_lines; //array for return path
            while (current_vertex != start) {
                path_lines.push_back(prev_line[current_vertex]);
                current_vertex = prev_vertex[current_vertex];
            }
            std::reverse(path_lines.begin(), path_lines.end());
            return { current_distance, path_lines }; //result
        }

        //Compute neighbors for curent vertex
        if (graph.find(current_vertex) != graph.end()) {
            for (const auto& neighbor : graph[current_vertex]) {
                float new_distance = current_distance + neighbor.weight;
                if (new_distance < dist[neighbor.to]) {
                    dist[neighbor.to] = new_distance;
                    prev_vertex[neighbor.to] = current_vertex;
                    prev_line[neighbor.to] = neighbor.line_index;
                    pq.push({ new_distance, neighbor.to });
                }
            }
        }
    }
    // INFINITY in no path
    return { std::numeric_limits<float>::infinity(), {} };
}