#include <iostream>

#include "glad.h"

#include "debugging.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>



std::ofstream fout("DebugLog.txt");
void Nullity::DebugLog(std::ostringstream& oss)
{
    fout << "---------------" << std::endl;
    fout << oss.str() << std::endl;
}

void Nullity::DebugLog(std::string& string)
{
    fout << "---------------" << std::endl;
    fout << string << std::endl;
}

void APIENTRY Nullity::glDebugOutput(GLenum source, 
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

    std::ostringstream oss;
    oss << "Debug message (" << id << "): " << message << std::endl;
    oss << sourceText << std::endl;
    oss << typeText << std::endl;
    oss << severityText << std::endl;

    DebugLog(oss);
}
