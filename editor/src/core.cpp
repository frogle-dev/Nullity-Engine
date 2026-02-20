#include "core.hpp"
#include "engine_gui.hpp"

#include <GLFW/glfw3.h>



NullityEditor::Editor::Editor(Nullity::Engine& engine)
	: state(engine)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

    InfoWindow(eng.state.msPerFrame, eng.state.fps);
    DebugOutputWindow();
    InspectorWindow(eng.data.registry);

    ImGui::Begin("Game");
    {
        ImGui::BeginChild("Render");

        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;

        state.framebuffer.Refresh(width, height);
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

    UtilityKeybinds(eng);
}

void NullityEditor::Editor::ExitFrame()
{
    state.framebuffer.Unbind();
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void NullityEditor::Editor::UtilityKeybinds(Nullity::Engine& eng)
{
    if (isActionJustPressed("wireframe"))
    {
        eng.state.wireframe = !eng.state.wireframe;
    }
}
