#include "init.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.hpp"
#include "debug_output.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"



bool init(GLFWwindow *&windowID, int width, int height)
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
        std::cout << "(Initialization): Error: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(windowID);
    
    glfwSetInputMode(windowID, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "(Initialization): Error: Failed to initialize GLAD" << std::endl;
        return false;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    return true;
}

void ImguiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // imgui styling
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 pink = ImVec4(0.725f, 0.451f, 0.459f, 1.0f);
    ImVec4 gray = ImVec4(0.176f, 0.176f, 0.204f, 1.0f);
    ImVec4 dark_gray = ImVec4(0.155f, 0.155f, 0.183f, 1.0f);
    ImVec4 light_pink = ImVec4(0.808f, 0.694f, 0.745f, 1.0f);
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 5.0f;
        style.ChildRounding = 2.5f;
        style.FrameRounding = 2.5f;
        style.TabBarOverlineSize = 0.0f;
        
        // windows
        style.Colors[ImGuiCol_TitleBg] = dark_gray;
        style.Colors[ImGuiCol_TitleBgActive] = dark_gray;
        style.Colors[ImGuiCol_TabActive] = light_pink;
        style.Colors[ImGuiCol_TabHovered] = light_pink;
        style.Colors[ImGuiCol_TabDimmed] = pink;
        style.Colors[ImGuiCol_TabDimmedSelected] = pink;
        style.Colors[ImGuiCol_Tab] = pink;
        style.Colors[ImGuiCol_TabUnfocused] = pink;
        style.Colors[ImGuiCol_TabUnfocusedActive] = pink;
        style.Colors[ImGuiCol_TabSelected] = light_pink;
        style.Colors[ImGuiCol_WindowBg] = gray;
        style.Colors[ImGuiCol_PopupBg] = gray;
        style.Colors[ImGuiCol_Border] = pink;
        style.Colors[ImGuiCol_ResizeGrip] = pink;
        style.Colors[ImGuiCol_ResizeGripActive] = light_pink;
        style.Colors[ImGuiCol_ResizeGripHovered] = light_pink;
        
        // menu bar
        style.Colors[ImGuiCol_MenuBarBg] = dark_gray;
        style.Colors[ImGuiCol_Header] = pink;
        style.Colors[ImGuiCol_HeaderHovered] = light_pink;
        style.Colors[ImGuiCol_HeaderActive] = light_pink;

        // list
        style.Colors[ImGuiCol_FrameBg] = dark_gray;

        // buttons
        style.Colors[ImGuiCol_Button] = pink;
        style.Colors[ImGuiCol_ButtonActive] = light_pink;
        style.Colors[ImGuiCol_ButtonHovered] = light_pink;
    }
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}
