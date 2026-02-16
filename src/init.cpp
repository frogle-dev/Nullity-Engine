#include "init.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <sstream>
#include <entt/entt.hpp>

#include "debugging.hpp"
#include "debugging.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


bool Engine::init(GLFWwindow *&windowID, int width, int height)
{
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // remove when testing performance and shipping
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    windowID = glfwCreateWindow(width, height, "First OpenGL", NULL, NULL);
    if (windowID == NULL)
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to create GLFW window" << std::endl;
        Engine::DebugLog(oss);
        
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(windowID);
    
    glfwSetInputMode(windowID, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to initialize GLAD" << std::endl;
        Engine::DebugLog(oss);
        return false;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(Engine::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    return true;
}

void Engine::ImguiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
       
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}
