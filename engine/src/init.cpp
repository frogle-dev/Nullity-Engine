#include "init.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "callbacks.hpp"
#include "state.hpp"
#include "debugging.hpp"


bool Nullity::Init(GLFWwindow *&windowID, State& engState)
{
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // remove when testing performance and shipping glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    windowID = glfwCreateWindow(engState.initViewRes.x, engState.initViewRes.y, "First OpenGL", NULL, NULL);
    if (windowID == NULL)
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to create GLFW window" << std::endl; Nullity::DebugLog(oss);
        
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(windowID);
    
    glfwSetInputMode(windowID, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to initialize GLAD" << std::endl;
        Nullity::DebugLog(oss);
        return false;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(Nullity::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSetWindowUserPointer(windowID, &engState);

    glfwSetWindowSizeCallback(windowID, WindowSizeCallback);
    glfwSetCursorPosCallback(windowID, MouseCallback);
    glfwSetKeyCallback(windowID, KeyCallback);

    return true;
}
