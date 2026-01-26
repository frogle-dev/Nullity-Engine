#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "components.hpp"

#include "shader.hpp"

#include <entt/entt.hpp>


void WorldObjectSystem(entt::registry& registry);