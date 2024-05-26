#pragma once
#include <string>

struct Point {
    float x, y;
    std::string name;
};

struct Line {
    int start_idx, end_idx;
    float mass;
    bool colored = false;
};

struct Edge {
    int to;
    float weight;
    int line_index;
};