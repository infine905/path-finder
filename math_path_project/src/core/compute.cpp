#include "compute.h"

std::tuple<float, std::vector<int>, std::vector<std::vector<float>>> find_path(const std::vector<Line>& lines, int start, int end) {

    //adjacency matrix staff
    int max_vertex = 0;
    for (const auto& line : lines) {
        if (line.start_idx > max_vertex) max_vertex = line.start_idx;
        if (line.end_idx > max_vertex) max_vertex = line.end_idx;
    }

    //Vars staff
    std::vector<std::vector<float>> adj_matrix(max_vertex + 1, std::vector<float>(max_vertex + 1, std::numeric_limits<float>::infinity())); //adjacency matrix with infinity
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> pq;                                      // priority queue by disntance
    std::unordered_map<int, float> dist;                                                                                                    //array for closet distances
    std::unordered_map<int, int> prev_vertex;                                                                                               //array for previous vertex
    std::unordered_map<int, int> prev_line;                                                                                                 //array for line indeces
    std::unordered_map<int, std::vector<Edge>> graph;
    std::set<int> vertices;

    //filling in the adjacency matrix
    for (int i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        graph[line.start_idx].push_back({ line.end_idx, line.mass, i });
        vertices.insert(line.start_idx);
        vertices.insert(line.end_idx);
        adj_matrix[line.start_idx][line.end_idx] = line.mass;
    }

    // init distance verteces
    for (int vertex : vertices) {
        dist[vertex] = std::numeric_limits<float>::infinity();
    }
    dist[start] = 0.0;

    pq.push({ 0.0, start });
    while (!pq.empty()) {
        float current_distance = pq.top().first;
        int current_vertex = pq.top().second;
        pq.pop();

        if (current_vertex == end) {
            std::vector<int> path_lines;
            while (current_vertex != start) {
                path_lines.push_back(prev_line[current_vertex]);
                current_vertex = prev_vertex[current_vertex];
            }
            std::reverse(path_lines.begin(), path_lines.end());
            return { current_distance, path_lines, adj_matrix };
        }

        //compute neighbor verteces
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

    //if no path retuurn infinity
    return { std::numeric_limits<float>::infinity(), {}, adj_matrix };
}