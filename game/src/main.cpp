#include "engine.hpp"

#ifdef USE_EDITOR
#include "editor.hpp"
namespace NE = NullityEditor;
#endif

#include "player.hpp"

namespace N = Nullity;
using namespace N::Components;

int main()
{
    N::EngineInit();
#ifdef USE_EDITOR
    NE::EditorInit();
#endif

    N::Entity dirt(N::registry);
    dirt.Add<DisplayName>("dirt")
        .Add<ObjectShader>(N::Data::unlitShader)
        .Add<Transform>()
        .Add<WorldObject>()
        .Add<ObjectModel>(N::Model("assets/models/Dirt/Dirt.obj"), true);

    N::Entity player(N::registry);
    player.Add<DisplayName>("player")
          .Add<Transform>()
          .Add<Player>()
          .Add<Velocity>()
          .Add<Camera>()
          .Add<Camera_Controller>();


    N::sys_Camera c;
    N::sys_Render r;
    while(N::Running())
    {
        N::EnterFrame();

        PlayerUpdate();
        CameraControls();

        c.Update();
        r.Update();
#ifdef USE_EDITOR
        NE::EnterFrame();
#endif

        N::RenderFramebuffer();
#ifdef USE_EDITOR
        NE::Update();
        NE::ExitFrame();
#endif

        N::ExitFrame();
    }

#ifdef USE_EDITOR
    NE::EditorExit();
#endif
    N::EngineExit();
    return 0;
}
