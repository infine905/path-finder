#pragma once
#pragma execution_character_set( "utf-8" )

#define IM_PI 3.14159265358979323846f // 300IQ fix MATH_DEFINES error

#include <vector>

#include <string>
#include <format>

#include <glfw3.h>
#include <imgui.h>

#include "ImStyle.h"
#include "../core/structs.h"
#include "../core/compute.h"
#include "../core/saveSystem.h"

//Settigs Staff
struct {
    ImU32 CanvasColor = IM_COL32(75, 75, 75, 255);

    float fPointSize = 5.f;
    ImU32 PointColor = IM_COL32(255, 0, 0, 255);
    ImU32 PointNameColor = IM_COL32(255, 255, 255, 255);

    float fLineArrowSize = 10.f;
    float fLineArrowthickness = 2.0f;
    ImU32 LineColor = IM_COL32(255, 195, 0, 255);
    ImU32 LineColoredColor = IM_COL32(0, 255, 0, 255);
    ImU32 LineMassColor = IM_COL32(60, 100, 255, 255);
    ImU32 LineMassColoredColor = IM_COL32(255, 0, 255, 255);
} Settings;

float distance = 0;
bool showMatrix = true;
std::string output;

ImVec2 PosToAddPoint;

std::vector<Point> points;
std::vector<Line> lines;
std::vector<int> path_lines;
std::vector<std::vector<float>> matrix;

void RenderUI(GLFWwindow* window, ImGuiIO& io) 
{
    //Window DockSpace Staff
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        if (ImGui::Begin("##DockSpace", (bool*)true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus))
        {
            ImGui::DockSpace(ImGui::GetID("DockSpace"));
            ImGui::End();
        }
        ImGui::PopStyleVar(2);
    }

    //Menu
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Файл")) {
            if (ImGui::MenuItem("Открыть")) {
                std::wstring file_path;
                OpenFileDialog(GetDesktopWindow(), file_path);
                loadFromFile(points, lines, file_path);
            }
            if (ImGui::MenuItem("Сохранить как...")) {
                std::wstring file_path;
                SaveFileDialog(GetDesktopWindow(), file_path);
                saveToFile(points, lines, file_path);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    //Work Space Window
    if (ImGui::Begin("Рабочая Область"), true, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking) {
        ImGui::Text("ЛКМ чтобы добавить точку. ПКМ чтобы очистить.");

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImVec2(ImGui::GetContentRegionAvail());
        if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
        if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        draw_list->AddRectFilled(canvas_p0, canvas_p1, Settings.CanvasColor);
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        bool is_hovered = ImGui::IsItemHovered();
        bool is_active = ImGui::IsItemActive();
        ImVec2 origin(canvas_p0.x, canvas_p0.y);
        ImVec2 mouse_pos_in_canvas(ImGui::GetIO().MousePos.x - origin.x, ImGui::GetIO().MousePos.y - origin.y);


        static char point_name[32];

        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImGui::OpenPopup("Add Point");
            PosToAddPoint = mouse_pos_in_canvas;
        }

        if (ImGui::BeginPopup("Add Point")) {
            ImGui::InputText("Название точки", point_name, IM_ARRAYSIZE(point_name));
            if (ImGui::Button("Добавить") && point_name[0] != '\0') {
                bool isPointExists = false;
                for (const auto& point : points) {
                    if (point.name == point_name) {
                        isPointExists = true;
                        break;
                    }
                }
                if (isPointExists) {
                    MessageBoxW(GetDesktopWindow(), L"Такая точка уже существует", L"Ошибка", MB_OK | MB_ICONHAND);
                }
                else {
                    points.push_back(Point{ PosToAddPoint.x, PosToAddPoint.y, point_name });
                    point_name[0] = '\0';
                }

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("##Clear Points");
        }

        if (ImGui::BeginPopup("##Clear Points")) {
            ImGui::Text("Очистить точки?");
            if (ImGui::Button("Очистить")) {
                points.clear();
                lines.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        for (const auto& point : points) {
            ImVec2 p = ImVec2(origin.x + point.x, origin.y + point.y);
            draw_list->AddCircleFilled(p, Settings.fPointSize, Settings.PointColor, 64);
            draw_list->AddText(ImVec2(p.x + 2, p.y + 5), Settings.PointNameColor, point.name.c_str());
        }

        for (const auto& line : lines) {
            char mass_str[255];
            sprintf_s(mass_str, "%g", line.mass);

            ImVec2 p_start = ImVec2(origin.x + points[line.start_idx].x, origin.y + points[line.start_idx].y);
            ImVec2 p_end = ImVec2(origin.x + points[line.end_idx].x, origin.y + points[line.end_idx].y);
            ImVec2 text_pos = ImVec2((p_start.x + p_end.x) * 0.5f, (p_start.y + p_end.y) * 0.5f);

            if (line.colored) {
                
                draw_list->AddText(ImVec2(text_pos.x - 1.f, text_pos.y - 1.f), IM_COL32(0, 0, 0, 255), mass_str);
                draw_list->AddText(text_pos, Settings.LineMassColoredColor, mass_str);
                DrawArrow(draw_list, p_start, p_end, Settings.LineColoredColor, Settings.fLineArrowthickness, Settings.fLineArrowSize);
            } else {
                DrawArrow(draw_list, p_start, p_end, Settings.LineColor, Settings.fLineArrowthickness, Settings.fLineArrowSize);
                draw_list->AddText(ImVec2(text_pos.x + 1.f, text_pos.y + 1.f), IM_COL32(0, 0, 0, 255), mass_str);
                draw_list->AddText(text_pos, Settings.LineMassColor, mass_str);
            }
        }
        ImGui::End();
    }

    //Tool Window
    if (ImGui::Begin("Инструменты"), true, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking) {

        static float connection_line_mass = 1.0f;
        static int connection_start_idx = -1;
        static int connection_end_idx = -1;

        static int compute_start_idx = -1;
        static int compute_end_idx = -1;

        ImGui::Text("Соеденить точки:"); ImGui::Separator();
        if (ImGui::BeginCombo("Стартвая Точка", connection_start_idx == -1 ? "Выбрать" : points[connection_start_idx].name.c_str())) {
            for (size_t i = 0; i < points.size(); ++i) {
                if (ImGui::Selectable(points[i].name.c_str())) {
                    connection_start_idx = i;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Конечная Точка", connection_end_idx == -1 ? "Выбрать" : points[connection_end_idx].name.c_str())) {
            for (size_t i = 0; i < points.size(); ++i) {
                if (ImGui::Selectable(points[i].name.c_str())) {
                    connection_end_idx = i;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputFloat("Масса Линии", &connection_line_mass, 0.1f, 0.1f, "%.1f");

        if (ImGui::Button("Соеденить") && connection_start_idx != -1 && connection_end_idx != -1 && connection_start_idx != connection_end_idx) {
            lines.push_back(Line{ connection_start_idx, connection_end_idx, connection_line_mass });
            connection_start_idx = -1;
            connection_end_idx = -1;
        }

        ImGui::Spacing();

        ImGui::Text("Расчитать Путь:"); ImGui::Separator();
        if (ImGui::BeginCombo("Из Точки", compute_start_idx == -1 ? "Выбрать" : points[compute_start_idx].name.c_str())) {
            for (size_t i = 0; i < points.size(); ++i) {
                if (ImGui::Selectable(points[i].name.c_str())) {
                    compute_start_idx = i;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("В Точку", compute_end_idx == -1 ? "Выбрать" : points[compute_end_idx].name.c_str())) {
            for (size_t i = 0; i < points.size(); ++i) {
                if (ImGui::Selectable(points[i].name.c_str())) {
                    compute_end_idx = i;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Расчитать") && compute_start_idx != -1 && compute_end_idx != -1 && compute_start_idx != compute_end_idx) {

            auto [shortest_path_mass, path_lines, adx_matrix] = find_path(lines, compute_start_idx, compute_end_idx);

            distance = shortest_path_mass;
            matrix = adx_matrix;
            for (size_t i = 0; i < lines.size(); ++i) {
                for (const auto& path_line : path_lines) {
                    if (i == path_line) {
                        lines[i].colored = true;
                        break;
                    } else {
                        lines[i].colored = false;
                    }
                }
            }

            output = std::format("Расстояние от точки {} до точки {}: {:g}", points[compute_start_idx].name.c_str(), points[compute_end_idx].name.c_str(), distance);

            if (distance == INFINITY) {
                MessageBoxW(GetDesktopWindow(), L"Такого пути нет", L"Ошибка", MB_OK | MB_ICONHAND);
            }
            else {
                MessageBoxW(GetDesktopWindow(), utf8ToUtf16(output).c_str(), L"Успех", MB_OK | MB_ICONASTERISK);
            }

            compute_start_idx = -1;
            compute_end_idx = -1;
        }

        //Display distance
        ImGui::Spacing(); ImGui::Separator(); {
            ImGui::Text(output.c_str());
        }

        ImGui::End();
    }

    if (ImGui::Begin("Матрица смежности", &showMatrix)) {
        int rows = matrix.size();
        int cols = rows > 0 ? matrix[0].size() : 0;

        if (ImGui::BeginTable("MatrixTable", cols + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg) && rows > 0) {
            // Отображение заголовков столбцов
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("");

            for (int col = 0; col < cols; ++col) {
                ImGui::TableSetColumnIndex(col + 1);
                ImGui::Text("%s",  points[col].name.c_str());
            }

            for (int row = 0; row < rows; ++row) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", points[row].name.c_str());

                for (int col = 0; col < cols; ++col) {
                    ImGui::TableSetColumnIndex(col + 1);
                    ImGui::PushID(row * cols + col);
                    ImGui::Text("%g", matrix[row][col]);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}