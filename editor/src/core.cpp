#include "core.hpp"
#include "engine_gui.hpp"
#include "engine.hpp"

#include <GLFW/glfw3.h>
#include <iostream>



NullityEditor::Editor::Editor(Nullity::Engine& engine)
	: state(engine)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(engine.window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

NullityEditor::Editor::~Editor()
{
	Cleanup();
}

void NullityEditor::Editor::Cleanup()
{
    state.framebuffer.Cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void NullityEditor::Editor::EnterFrame()
{
    state.framebuffer.Bind();
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void NullityEditor::Editor::Update(Nullity::Engine& eng)
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
                state.demoWindow = !state.demoWindow;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (state.demoWindow)
    {
        ImGui::ShowDemoWindow();
    }

    InfoWindow(eng.time.msPerFrame, eng.time.fps, eng.input);
    DebugOutputWindow();
    InspectorWindow(eng.registry);

    ImGui::Begin("Game");
    {
        ImGui::BeginChild("Render");

        ImVec2 reg = ImGui::GetContentRegionAvail();

        Nullity::WindowSizeCallback(eng.window, reg.x, reg.y);
        state.framebuffer.Refresh(reg.x, reg.y);

        ImGui::Image(
            (ImTextureID)state.framebuffer.GetColorTexture(), 
            reg, 
            ImVec2(0, 1), 
            ImVec2(1, 0)
        );
    }
    ImGui::EndChild();
    ImGui::End();

    ImGui::Render();

    UtilityKeybinds(eng);
}

void NullityEditor::Editor::ExitFrame()
{
    state.framebuffer.Unbind();
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

void NullityEditor::Editor::UtilityKeybinds(Nullity::Engine& eng)
{
    if (eng.input.isActionJustPressed("wireframe"))
    {
        eng.state.wireframe = !eng.state.wireframe;
    }
}
