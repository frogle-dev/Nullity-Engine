A simple WIP game engine

Requirements:
- glfw3
- assimp
- hardware that supports opengl 4.6

### How to use

```
#include "engine.hpp"

int main()
{
    Nullity::Engine Engine;

    while(!glfwWindowShouldClose(Engine.window))
    {
        Engine.EnterFrame();



        Engine.Render(camera);
        Engine.RenderFramebuffer();
        Engine.ExitFrame();
    }

    return 0;
}
```
