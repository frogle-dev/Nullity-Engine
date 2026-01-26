#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.hpp"
#include "debugging.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


bool init(GLFWwindow *&windowID, int width, int height);

void ImguiInit(GLFWwindow* window);
