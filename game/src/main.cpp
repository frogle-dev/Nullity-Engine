#include "engine.hpp"

#ifdef USE_EDITOR
#include "editor.hpp"
#endif

#include "player.hpp"

namespace N = Nullity;
using namespace N::Components;

int main()
{
    N::EngineInit();
#ifdef USE_EDITOR
    NullityEditor::Editor Editor(Engine);
#endif

    N::Camera camera;

    N::Entity dirt(N::GetRegistry());
    dirt.Add<DisplayName>("dirt")
        .Add<ObjectShader>(N::Data::unlitShader)
        .Add<Transform>()
        .Add<WorldObject>()
        .Add<ObjectModel>(N::Model("assets/models/Dirt/Dirt.obj"), true);

    N::Entity player(N::GetRegistry());
    player.Add<DisplayName>("player")
          .Add<Transform>()
          .Add<Player>()
          .Add<Velocity>();

    while(N::Running())
    {
        N::EnterFrame();

        PlayerUpdate(N::GetRegistry(), camera);
        CameraControls(camera);

        N::Render(camera);
#ifdef USE_EDITOR
        Editor.EnterFrame();
#endif

        N::RenderFramebuffer();
#ifdef USE_EDITOR
        Editor.Update(Engine);
        Editor.ExitFrame();
#endif

        N::ExitFrame();
    }

    N::EngineClose();
    return 0;
}
