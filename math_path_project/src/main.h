#pragma once
#include <glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <Windows.h>

#include "ui/ui.h"

#pragma comment(lib, "opengl32.lib")

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int Main(int argc, char** argv);