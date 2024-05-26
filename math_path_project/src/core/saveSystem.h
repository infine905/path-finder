#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <Windows.h>

#include "structs.h"

bool SaveFileDialog(HWND owner, std::wstring& filePath);
bool OpenFileDialog(HWND owner, std::wstring& filePath);
void saveToFile(const std::vector<Point>& points, const std::vector<Line>& lines, const std::wstring& filename);
void loadFromFile(std::vector<Point>& points, std::vector<Line>& lines, const std::wstring& filename);