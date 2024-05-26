#include "saveSystem.h"

bool SaveFileDialog(HWND owner, std::wstring& filePath) {
    OPENFILENAME ofn;
    WCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"Binary Files\0*.bin\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"bin";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        filePath = ofn.lpstrFile;
        return true;
    }

    return false;
}

bool OpenFileDialog(HWND owner, std::wstring& filePath) {
    OPENFILENAME ofn;
    WCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


    if (GetOpenFileName(&ofn) == TRUE) {
        filePath = ofn.lpstrFile;
        return true;
    }

    return false;
}

void saveToFile(const std::vector<Point>& points, const std::vector<Line>& lines, const std::wstring& filename) {
    std::ofstream ofs(filename, std::ios::binary);

    if (!ofs) {
        std::wcerr << L"Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Save points
    size_t points_size = points.size();
    ofs.write(reinterpret_cast<const char*>(&points_size), sizeof(points_size));

    for (const auto& point : points) {
        ofs.write(reinterpret_cast<const char*>(&point.x), sizeof(point.x));
        ofs.write(reinterpret_cast<const char*>(&point.y), sizeof(point.y));

        size_t name_size = point.name.size();
        ofs.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
        ofs.write(point.name.c_str(), name_size);
    }

    // Save lines
    size_t lines_size = lines.size();
    ofs.write(reinterpret_cast<const char*>(&lines_size), sizeof(lines_size));

    for (const auto& line : lines) {
        ofs.write(reinterpret_cast<const char*>(&line.start_idx), sizeof(line.start_idx));
        ofs.write(reinterpret_cast<const char*>(&line.end_idx), sizeof(line.end_idx));
        ofs.write(reinterpret_cast<const char*>(&line.mass), sizeof(line.mass));
    }

    ofs.close();
}

void loadFromFile(std::vector<Point>& points, std::vector<Line>& lines, const std::wstring& filename) {
    std::ifstream ifs(filename, std::ios::binary);

    if (!ifs) {
        std::wcerr << L"Error opening file for reading: " << filename << std::endl;
        return;
    }

    // Load points
    size_t points_size;
    ifs.read(reinterpret_cast<char*>(&points_size), sizeof(points_size));
    points.resize(points_size);

    for (auto& point : points) {
        ifs.read(reinterpret_cast<char*>(&point.x), sizeof(point.x));
        ifs.read(reinterpret_cast<char*>(&point.y), sizeof(point.y));

        size_t name_size;
        ifs.read(reinterpret_cast<char*>(&name_size), sizeof(name_size));

        point.name.resize(name_size);
        ifs.read(&point.name[0], name_size);
    }

    // Load lines
    size_t lines_size;
    ifs.read(reinterpret_cast<char*>(&lines_size), sizeof(lines_size));
    lines.resize(lines_size);

    for (auto& line : lines) {
        ifs.read(reinterpret_cast<char*>(&line.start_idx), sizeof(line.start_idx));
        ifs.read(reinterpret_cast<char*>(&line.end_idx), sizeof(line.end_idx));
        ifs.read(reinterpret_cast<char*>(&line.mass), sizeof(line.mass));
    }

    ifs.close();
}