#pragma once
#include <set>
#include <queue>
#include <limits>
#include <unordered_map>

#include "structs.h"

std::tuple<float, std::vector<int>, std::vector<std::vector<float>>> find_path(const std::vector<Line>& lines, int start, int end);