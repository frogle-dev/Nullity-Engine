#define STB_IMAGE_IMPLEMENTATION

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "shader.hpp"
#include "keymap.hpp"
#include "camera.hpp"
#include "textures.hpp"
#include "framebuffer.hpp"

#include "engine.hpp"
#include "engine_gui.hpp"

#include "init.hpp"
#include "systems.hpp"
#include "player.hpp"
#include "render.hpp"


void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


int main()
{
    Engine::State App;
    Engine::MouseState mouseState;
    App.mouse = &mouseState;

    mouseState.lastMousePos = App.initViewRes / 2;

    Camera camera;


    GLFWwindow* window;
    if (!Engine::init(window, App.initViewRes.x, App.initViewRes.y))
        return -1;
 
    glfwSetWindowUserPointer(window, &App);

    Framebuffer gameFrameBuffer(App.viewRes.x, App.viewRes.y);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);


    Engine::ImguiInit(window);
    ImGuiIO& io = ImGui::GetIO();

    float accent1[4] = {251.0f/255, 103.0f/255, 255.0f/255, 255.0f/255};
    float accent2[4] = {251.0f/255, 103.0f/255, 255.0f/255, 100.0f/255};
    float bg1[4] = {60.0f/255, 60.0f/255, 60.0f/255, 255.0f/255};
    float bg2[4] = {0.0f/255, 0.0f/255, 0.0f/255, 84.0f/255};


    Engine::Data Engine;

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

    Engine::Scene Scene;
    Scene.LoadObjects(Engine);


    gameFrameBuffer.Unbind();
    
    bool demoWindow = false;

    float deltaTime = 0.0f;
    int fps;
    float msPerFrame;
    float lastFrame = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        msPerFrame = deltaTime * 1000;
        fps = 1000 / msPerFrame;


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Demo Window")) 
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
        
        Engine::InfoWindow(msPerFrame, fps);
        Engine::DebugOutputWindow();
        Engine::InspectorWindow(Engine.registry);

        ImGui::Begin("Game");
        {
            ImGui::BeginChild("Render");
            
            float width = ImGui::GetContentRegionAvail().x;
            float height = ImGui::GetContentRegionAvail().y;
            
            gameFrameBuffer.Rescale(width, height);
            window_size_callback(window, width, height);

            ImGui::Image(
                (ImTextureID)gameFrameBuffer.GetColorTexture(), 
                ImGui::GetContentRegionAvail(), 
                ImVec2(0, 1), 
                ImVec2(1, 0)
            );
        }
        ImGui::EndChild();
        ImGui::End();

        Engine::Styling(accent1, accent2, bg1, bg2);

        ImGui::Render();


        gameFrameBuffer.Bind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color + depth buffer
        glEnable(GL_DEPTH_TEST);


        // game loop stuff
        Engine::UtilityKeybinds(window, App);
        PlayerUpdate(Engine.registry, camera, deltaTime);
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
        gameFrameBuffer.Unbind();
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
    gameFrameBuffer.Cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    Engine::State* state = static_cast<Engine::State*>(glfwGetWindowUserPointer(window));

    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)state->initViewRes.x / state->initViewRes.y;

    if (aspect > targetAspect)
    {
        state->viewRes.y = height;
        state->viewRes.x = (int)(height * targetAspect);
    }
    else
    {
        state->viewRes.x = width;
        state->viewRes.y = (int)(width / targetAspect);
    }

    int viewX = (width - state->viewRes.x) / 2;
    int viewY = (height - state->viewRes.y) / 2;

    glViewport(viewX, viewY, state->viewRes.x, state->viewRes.y);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Engine::State* state = static_cast<Engine::State*>(glfwGetWindowUserPointer(window));

    state->mouse->mousePos = glm::dvec2(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    processKeyEvent(scancode, action);
}
