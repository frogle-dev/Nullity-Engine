#define STB_IMAGE_IMPLEMENTATION

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "framebuffer.hpp"

#include "engine.hpp"
#include "engine_gui.hpp"

#include "init.hpp"
#include "player.hpp"
#include "render.hpp"



int main()
{
    Nullity::State App;
    Nullity::MouseState mouseState;
    App.mouse = &mouseState;

    mouseState.lastMousePos = App.initViewRes / 2;

    Camera camera;


    GLFWwindow* window;
    if (!Nullity::Init(window, App))
        return -1;
 
    Nullity::Data Engine;
    Nullity::GUIData GuiData(App);

    Nullity::ImguiInit(window);
    ImGuiIO& io = ImGui::GetIO();

    float accent1[4] = {251.0f/255, 103.0f/255, 255.0f/255, 255.0f/255};
    float accent2[4] = {251.0f/255, 103.0f/255, 255.0f/255, 100.0f/255};
    float bg1[4] = {60.0f/255, 60.0f/255, 60.0f/255, 255.0f/255};
    float bg2[4] = {0.0f/255, 0.0f/255, 0.0f/255, 84.0f/255};
    Nullity::Styling(accent1, accent2, bg1, bg2);

    reloadConfigKeymaps();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // glEnable(GL_BLEND); 
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // textures
    glActiveTexture(GL_TEXTURE0);

    TextureManager::Get().GenerateTextureArray(4096, 4096, 100, Engine.texArrayDataUBO);
    
    GLuint texArrayID = TextureManager::Get().GetTexArrayID();

    Engine.objectShader.use();
    Engine.objectShader.setFloat("material.emissionStrength", 1.0f);
    Engine.objectShader.setFloat("material.shininess", 128.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureManager::Get().GenerateMipmaps(); // generate texture array mipmaps once all textures have been loaded in
    TextureManager::Get().SendSubTexResArrayToShader(Engine.texArrayDataUBO); // send the tex res array to the frag shader


    auto dirt = Engine.registry.create();
    Engine.registry.emplace<DisplayName>(dirt, "dirt");
    Engine.registry.emplace<ObjectShader>(dirt, Engine.unlitShader);
    Engine.registry.emplace<WorldObject>(dirt);
    Engine.registry.emplace<ObjectModel>(dirt, Model("models/Dirt/Dirt.obj"), true);

    auto player = Engine.registry.create();
    Engine.registry.emplace<DisplayName>(player, "player");
    Engine.registry.emplace<Transform>(player);
    Engine.registry.emplace<Player>(player);
    Engine.registry.emplace<Velocity>(player);


    GuiData.frameBuffer.Unbind();
    
    while(!glfwWindowShouldClose(window))
    {
        Nullity::UpdateEngine(App);
        Nullity::UpdateEngineGUI(App, GuiData, Engine, window);


        GuiData.frameBuffer.Bind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color + depth buffer
        glEnable(GL_DEPTH_TEST);


        // game loop stuff
        Nullity::UtilityKeybinds(window, App);
        PlayerUpdate(Engine.registry, camera, App.deltaTime);
        CameraControls(mouseState, App, camera);


        Engine.objectShader.use();
        Engine.objectShader.setVec3("viewPos", camera.position);

        TextureManager::Get().SendSubTexResArrayToShader(Engine.texArrayDataUBO); // send the tex res array to the frag shader

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)App.viewRes.x / App.viewRes.y, 0.1f, 1000.0f);

        SetUniformBufferData(Engine.matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(Engine.matricesUBO, 64, 64, glm::value_ptr(projection));
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Engine.skyboxCubemap); // binding skybox for reflections

        WorldObjectSystem(Engine.registry);
        DrawSystem(Engine.registry);

        // skybox
        glDepthFunc(GL_LEQUAL);
        Engine.skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        SetUniformBufferData(Engine.matricesUBO, 0, 64, glm::value_ptr(view));

        glBindVertexArray(Engine.skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Engine.skyboxCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);

        // rendering to framebuffer
        GuiData.frameBuffer.Unbind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // end of process life
    Engine.Cleanup();
    GuiData.frameBuffer.Cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
