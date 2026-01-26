#pragma once

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "glad.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>


void DebugOutputWindow();

void DebugLog(std::ostringstream& oss);

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);