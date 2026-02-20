A simple WIP game engine

Requirements:
- glfw3
- assimp
- hardware that supports opengl 4.6

### How to use

```c++
#include "engine.hpp"

int main()
{
    Nullity::Engine Engine; // Create the engine

    while(!glfwWindowShouldClose(Engine.window)) // Game loop
    {
        Engine.EnterFrame();

        // Add your game updates here

        Engine.Render(camera);
        Engine.RenderFramebuffer();
        Engine.ExitFrame();
    }

    return 0;
}
```
