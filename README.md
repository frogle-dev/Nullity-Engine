A simple WIP game engine

Requirements:
- glfw3
- assimp
- hardware that supports opengl 4.6

### Installation

<details>
<summary>Linux</summary>
<br>
d
</details>

<details>
<summary>Windows</summary>
<br>

- Install [visual studio](https://visualstudio.microsoft.com/downloads/) or the vs build tools that are at the bottom of the page under "Tools for visual studio"
    - Choose "Desktop development with c++" in the installer 
- Install cmake at: [https://cmake.org/](https://cmake.org/) (make sure add to path is on during the installation)
- Install [git](https://git-scm.com/)

If you are using Visual Studio, open "x64 Native Tools Command Prompt for VS"

If using vs build tools open a normal terminal


Run:
```
git clone https://github.com/frogle-dev/Nullity-Engine.git
cd Nullity-Engine
```

And then you can run the ```run.bat``` script to build the engine

</details>

-------------

### Game
Use the game folder as a template to work off of

### Using the engine

```c++
#include "engine.hpp"

int main()
{
    Nullity::Engine Engine; // Create the engine

    while(Engine.Running()) // Game loop
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

    while(Engine.Running())
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
