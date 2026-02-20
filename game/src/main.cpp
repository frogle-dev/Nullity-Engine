#include "engine.hpp"

#ifdef USE_EDITOR // no editor in release mode #include "editor.hpp"
#include "editor.hpp"
#endif

#include "player.hpp"


int main()
{
    Nullity::Engine Engine;

#ifdef USE_EDITOR 
    NullityEditor::Editor Editor(Engine);
#endif

    Camera camera;

    auto dirt = Engine.registry.create();
    Engine.registry.emplace<DisplayName>(dirt, "dirt");
    Engine.registry.emplace<ObjectShader>(dirt, Engine.data.unlitShader);
    Engine.registry.emplace<Transform>(dirt);
    Engine.registry.emplace<WorldObject>(dirt);
    Engine.registry.emplace<ObjectModel>(dirt, Model("assets/models/Dirt/Dirt.obj"), true);

    auto player = Engine.registry.create();
    Engine.registry.emplace<DisplayName>(player, "player");
    Engine.registry.emplace<Transform>(player);
    Engine.registry.emplace<Player>(player);
    Engine.registry.emplace<Velocity>(player);


    while(!glfwWindowShouldClose(Engine.window))
    {
        Engine.EnterFrame();

        PlayerUpdate(Engine.registry, camera, Engine.state.deltaTime);
        CameraControls(Engine.state.mouse, Engine.state, camera);

        Engine.Render(camera);
        
#ifdef USE_EDITOR
        Editor.EnterFrame();
#endif

        Engine.RenderFramebuffer();

#ifdef USE_EDITOR
        Editor.Update(Engine);
        Editor.ExitFrame();
#endif

        Engine.ExitFrame();
    }

    return 0;
}
