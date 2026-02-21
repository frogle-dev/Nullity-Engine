#include "engine.hpp"

#ifdef USE_EDITOR
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

    Nullity::Entity dirt(Engine.registry);
    dirt.Add<DisplayName>("dirt")
        .Add<ObjectShader>(Engine.data.unlitShader)
        .Add<Transform>()
        .Add<WorldObject>()
        .Add<ObjectModel>(Model("assets/models/Dirt/Dirt.obj"), true);

    Nullity::Entity player(Engine.registry);
    player.Add<DisplayName>("player")
        .Add<Transform>()
        .Add<Player>()
        .Add<Velocity>();

    while(Engine.Running())
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
