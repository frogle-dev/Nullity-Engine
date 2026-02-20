A simple WIP game engine

Requirements:
- glfw3
- assimp
- hardware that supports opengl 4.6


### Using the engine

```c++
#include "engine.hpp"

int main()
{
    Nullity::Engine Engine; // Create the engine

    while(!glfwWindowShouldClose(Engine.window)) // Game loop
    {
        Engine.EnterFrame();

        // Game updates

        Engine.Render(camera);
        Engine.RenderFramebuffer();
        Engine.ExitFrame();
    }

    return 0;
}
```

### Using the editor

```c++
#include "engine.hpp"
#include "editor.hpp"

int main()
{
    Nullity::Engine Engine;
    NullityEditor::Editor Editor(Engine); // Create the editor

    while(!glfwWindowShouldClose(Engine.window))
    {
        Engine.EnterFrame();

        // Game updates

        Engine.Render(camera);
        
        Editor.EnterFrame();

        Engine.RenderFramebuffer();

        Editor.Update(Engine);
        Editor.ExitFrame();

        Engine.ExitFrame();
    }

    return 0;
}
```
