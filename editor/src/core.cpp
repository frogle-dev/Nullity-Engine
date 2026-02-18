#include "core.hpp"
#include "engine_gui.hpp"
#include "editor_init.hpp"

#include <GLFW/glfw3.h>


NullityEditor::Editor::Editor(GLFWwindow* window, Nullity::Engine& engine)
	: state(engine)
{
	Init(window, engine);
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
