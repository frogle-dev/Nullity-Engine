#include "engine.hpp"

#ifdef USE_EDITOR
#include "editor.hpp"
#endif

#include "player.hpp"

using namespace Nullity::Components;


int main()
{
    Nullity::Engine Engine;
#ifdef USE_EDITOR
    NullityEditor::Editor Editor(Engine);
#endif

    Nullity::Camera camera;

    Nullity::Entity dirt(Engine.registry);
    dirt.Add<DisplayName>("dirt")
        .Add<ObjectShader>(Engine.data.unlitShader)
        .Add<Transform>()
        .Add<WorldObject>()
        .Add<ObjectModel>(Nullity::Model("assets/models/Dirt/Dirt.obj"), true);

    Nullity::Entity player(Engine.registry);
    player.Add<DisplayName>("player")
        .Add<Transform>()
        .Add<Player>()
        .Add<Velocity>();

    while(Engine.Running())
    {
        Engine.EnterFrame();

        PlayerUpdate(Engine.registry, camera, Engine.time.deltaTime, Engine.input);
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
