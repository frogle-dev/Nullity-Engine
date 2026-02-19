#include "core.hpp"
#include "engine_gui.hpp"
#include "editor_init.hpp"

#include <GLFW/glfw3.h>


NullityEditor::Editor::Editor(Nullity::Engine& engine)
	: state(engine)
{
	Init(engine.window);
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

    InfoWindow(eng.state.msPerFrame, eng.state.fps);
    DebugOutputWindow();
    InspectorWindow(eng.data.registry);

    ImGui::Begin("Game");
    {
        ImGui::BeginChild("Render");

        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;

        state.framebuffer.Rescale(width, height);
        Nullity::WindowSizeCallback(eng.window, width, height);

        ImGui::Image(
            (ImTextureID)state.framebuffer.GetColorTexture(), 
            ImGui::GetContentRegionAvail(), 
            ImVec2(0, 1), 
            ImVec2(1, 0)
        );
    }
    ImGui::EndChild();
    ImGui::End();

    ImGui::Render();
}
