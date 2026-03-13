#pragma once

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include <unordered_map>
#include <vector>
#include <string>

namespace Nullity
{
    namespace Input
    {
        struct ActionState 
        {
            bool pressed = false;
            bool justPressed = false;
            bool released = false;
        };

        void InputInit();

        inline glm::vec2 lastMousePos = glm::vec2(0);
        inline bool firstMouse = true;
        inline glm::dvec2 mousePos = glm::dvec2(0);
        // called by glfw key callback
        // returns all keymaps that have been set from "ReloadConfigKeymaps()"
        const std::unordered_map<std::string, std::vector<int>>& GetConfigKeymaps();
        int CurrentScancodePressed();


        void KeysRefresh(); 
        bool IsActionPressed(const std::string& actionName); 
        bool IsActionJustPressed(const std::string& actionName);
        bool IsActionReleased(const std::string& actionName);
        void ProcessKeyEvent(int scancode, int action); 
        // reads and loads all keymaps from keymaps.json (run whenever keymaps.json is changed)
        void ReloadConfigKeymaps();
        // sets an already existing mapping to another key, or creates a new one in keymaps.json. 
        //bool 'addkeycode' = true, adds the keycode to the json file, as false it changes a keycode at 'index'
        void SetConfigKeymap(const std::string& actionName, bool addKeycode, int keycode, int index = 0);
        // removes an already existing mapping in keymaps.json. 
        void RemoveConfigKeymap(const std::string& actionName, int index);


        // Processes mouse inputs
        void MouseCallback(GLFWwindow* window, double xpos, double ypos);
        // Processes key inputs
        void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
