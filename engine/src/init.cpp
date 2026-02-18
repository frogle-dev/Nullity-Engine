#include "init.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include "callbacks.hpp"
#include "textures.hpp"
#include "state.hpp"
#include "debugging.hpp"


bool GlfwOpenGLInit(GLFWwindow* window, Nullity::State& engState)
{
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // remove when testing performance and shipping 
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(engState.initViewRes.x, engState.initViewRes.y, "First OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to create GLFW window" << std::endl; Nullity::DebugLog(oss);
        
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to initialize GLAD" << std::endl;
        Nullity::DebugLog(oss);
        return false;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(Nullity::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSetWindowUserPointer(window, &engState);

    glfwSetWindowSizeCallback(window, Nullity::WindowSizeCallback);
    glfwSetCursorPosCallback(window, Nullity::MouseCallback);
    glfwSetKeyCallback(window, Nullity::KeyCallback);

    return true;
}

bool TexturesInit(Nullity::Data& engData)
{
    glActiveTexture(GL_TEXTURE0);

    TextureManager::Get().GenerateTextureArray(4096, 4096, 100, engData.texArrayDataUBO);
    
    GLuint texArrayID = TextureManager::Get().GetTexArrayID();

    engData.objectShader.use();
    engData.objectShader.setFloat("material.emissionStrength", 1.0f);
    engData.objectShader.setFloat("material.shininess", 128.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureManager::Get().GenerateMipmaps();
    TextureManager::Get().SendSubTexResArrayToShader(engData.texArrayDataUBO);

    return true;
}

bool Nullity::Init(GLFWwindow* window)
{
    State engState;

    engState.mouse.lastMousePos = engState.initViewRes / 2;

    if (!GlfwOpenGLInit(window, engState))
        return false;


    reloadConfigKeymaps();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Nullity::Data engData;
    TexturesInit(engData);

    return true;
}
