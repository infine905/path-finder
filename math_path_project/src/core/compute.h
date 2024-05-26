#pragma once
#include <set>
#include <queue>
#include <limits>
#include <unordered_map>

#include "structs.h"

std::pair<float, std::vector<int>> find_path(const std::vector<Line>& lines, int start, int end);