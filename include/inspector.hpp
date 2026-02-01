#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <entt/entt.hpp>

#include "components.hpp"


void InspectorWindow(entt::registry& registry);
