#include "engine.hpp"

#ifdef DEBUG // no editor in release mode #include "editor.hpp"
#include "editor.hpp"
#endif

#include "player.hpp"

#include <iostream>


int main()
{
    Nullity::Engine Engine;
    NullityEditor::Editor Editor(Engine);

    ImGuiIO& io = ImGui::GetIO();

    Camera camera;

    // auto dirt = Engine.registry.create();
    // Engine.registry.emplace<DisplayName>(dirt, "dirt");
    // Engine.registry.emplace<ObjectShader>(dirt, Engine.unlitShader);
    // Engine.registry.emplace<WorldObject>(dirt);
    // Engine.registry.emplace<ObjectModel>(dirt, Model("models/Dirt/Dirt.obj"), true);

    // auto player = Engine.registry.create();
    // Engine.registry.emplace<DisplayName>(player, "player");
    // Engine.registry.emplace<Transform>(player);
    // Engine.registry.emplace<Player>(player);
    // Engine.registry.emplace<Velocity>(player);


    Editor.state.framebuffer.Unbind();

    while(!glfwWindowShouldClose(Engine.window))
    {
        Engine.Update();
        Editor.Update(Engine);

        Editor.state.framebuffer.Bind();
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color + depth buffer
        glEnable(GL_DEPTH_TEST);


        // game loop stuff
        Nullity::UtilityKeybinds(Engine.window, Engine.state);
        PlayerUpdate(Engine.data.registry, camera, Engine.state.deltaTime);
        CameraControls(Engine.state.mouse, Engine.state, camera);


        Engine.data.objectShader.use();
        Engine.data.objectShader.setVec3("viewPos", camera.position);

        TextureManager::Get().SendSubTexResArrayToShader(Engine.data.texArrayDataUBO); // send the tex res array to the frag shader

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)Engine.state.viewRes.x / Engine.state.viewRes.y, 0.1f, 1000.0f);

        SetUniformBufferData(Engine.data.matricesUBO, 0, 64, glm::value_ptr(view));
        SetUniformBufferData(Engine.data.matricesUBO, 64, 64, glm::value_ptr(projection));
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Engine.data.skyboxCubemap); // binding skybox for reflections

        WorldObjectSystem(Engine.data.registry);
        DrawSystem(Engine.data.registry);

        // skybox
        glDepthFunc(GL_LEQUAL);
        Engine.data.skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        SetUniformBufferData(Engine.data.matricesUBO, 0, 64, glm::value_ptr(view));

        glBindVertexArray(Engine.data.skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Engine.data.skyboxCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);

        // rendering to framebuffer
        Editor.state.framebuffer.Unbind();
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

        glfwSwapBuffers(Engine.window);
        glfwPollEvents();
    }

    return 0;
}
