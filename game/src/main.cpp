#include "engine.hpp"

#ifdef DEBUG // no editor in release mode #include "editor.hpp"
#include "editor.hpp"
#endif

#include "player.hpp"


int main()
{
    Nullity::Engine Engine;

#ifdef DEBUG 
    NullityEditor::Editor Editor(Engine);
#endif

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


    while(!glfwWindowShouldClose(Engine.window))
    {
        Engine.EnterFrame();

        PlayerUpdate(Engine.data.registry, camera, Engine.state.deltaTime);
        CameraControls(Engine.state.mouse, Engine.state, camera);


        Engine.Render(camera);
        
#ifdef DEBUG
        Editor.EnterFrame();
#endif

        Engine.RenderFramebuffer();

#ifdef DEBUG
        Editor.Update(Engine);
        Editor.ExitFrame();
#endif


        Engine.ExitFrame();
    }

    return 0;
}
