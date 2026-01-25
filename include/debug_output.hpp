#pragma once

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "glad.h"

#include <vector>
#include <string>
#include <fstream>


void DebugOutputWindow()
{
    ImGui::Begin("Debug Output");

    std::ifstream fin;
    fin.open("DebugLog.txt");

    if (ImGui::BeginListBox("Debug"))
    {
        while(!fin.eof())
        {
            std::string line;
            std::getline(fin, line);
            ImGui::TextWrapped("%s", line.c_str());
        }

        ImGui::EndListBox();
    }

    fin.close();

    ImGui::End();
}

void OutputDebugText(std::ostream& out, std::vector<std::string>& lines)
{
    out << "---------------" << std::endl;

    for (auto& line : lines)
    {
        out << line << std::endl;
    }
}

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    
    std::string sourceText;
    std::string typeText;
    std::string severityText;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             sourceText = "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceText = "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceText = "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceText = "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     sourceText = "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           sourceText = "Source: Other"; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               typeText = "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeText = "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeText = "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         typeText = "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeText = "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              typeText = "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          typeText = "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           typeText = "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               typeText = "Type: Other"; break;
    }
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         severityText = "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severityText = "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severityText = "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityText = "Severity: notification"; break;
    }

    std::vector<std::string> lines;
    lines.push_back("Debug message (" + std::to_string(id) + "): " +  message);
    lines.push_back(sourceText);
    lines.push_back(typeText);
    lines.push_back(severityText);

    std::ofstream fout;
    fout.open("DebugLog.txt");
    OutputDebugText(fout, lines);
    fout.close();
    OutputDebugText(std::cout, lines);
}
