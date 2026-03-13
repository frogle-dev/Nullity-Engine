#include "core.hpp"
#include "engine_gui.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace N = Nullity;
namespace NE = NullityEditor;

void NE::EditorInit()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(N::window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    framebuffer = N::Framebuffer(N::initViewRes.x, N::initViewRes.y);
}

void NE::EditorExit()
{
    framebuffer.Cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void NE::EnterFrame()
{
    framebuffer.Bind();
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void NE::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Imgui Demo Window"))
            {
                demoWindow = !demoWindow;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (demoWindow)
    {
        ImGui::ShowDemoWindow();
    }

    InfoWindow(N::Time::msPerFrame, N::Time::fps);
    DebugOutputWindow();
    InspectorWindow();

    ImGui::Begin("Game");
    {
        ImGui::BeginChild("Render");

        ImVec2 reg = ImGui::GetContentRegionAvail();

        Nullity::WindowSizeCallback(N::window, reg.x, reg.y);
        framebuffer.Refresh(reg.x, reg.y);

        ImGui::Image(
            (ImTextureID)framebuffer.GetColorTexture(), 
            reg, 
            ImVec2(0, 1), 
            ImVec2(1, 0)
        );
    }
    ImGui::EndChild();
    ImGui::End();

    ImGui::Render();

    UtilityKeybinds();
}

void NE::ExitFrame()
{
    framebuffer.Unbind();
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupCurrentContext);
    }
}

void NE::UtilityKeybinds()
{
    if (N::Input::IsActionJustPressed("wireframe"))
    {
        N::wireframe = !N::wireframe;
    }
}
