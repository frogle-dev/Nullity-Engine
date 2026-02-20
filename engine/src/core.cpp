#include "core.hpp"
#include "state.hpp"
#include "debugging.hpp"
#include "textures.hpp"
#include "callbacks.hpp"
#include "systems.hpp"
#include "render.hpp"
#include "primitives.hpp"

#include <GLFW/glfw3.h>

#include <iostream>


Nullity::Engine::Engine()
{
	if(!Init())
	{
        std::ostringstream oss;
        oss << "Engine failed to initialize" << std::endl;
        Nullity::DebugLog(oss);
	}

    // render texture setup
    framebuffer = Framebuffer(state.viewRes.x, state.viewRes.y);

    GLuint renderTexVBO;
    glGenVertexArrays(1, &renderTexVAO);
    glGenBuffers(1, &renderTexVBO);
    glBindVertexArray(renderTexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderTexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_quad), &vertices_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

Nullity::Engine::~Engine()
{
	data.Cleanup();
    framebuffer.Cleanup();

    glfwTerminate();
}

void Nullity::Engine::UtilityKeybinds()
{
    if (isActionJustPressed("focus"))
    {
        state.focus = !state.focus;
    }
}

void Nullity::Engine::EnterFrame()
{
    if (state.focus)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (state.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float currentFrame = glfwGetTime();
    state.deltaTime = currentFrame - state.lastFrame;
    state.lastFrame = currentFrame;
    
    state.msPerFrame = state.deltaTime * 1000;
    state.fps = 1000 / state.msPerFrame;


    framebuffer.Bind();
    glViewport(0,0,state.viewRes.x,state.viewRes.y);
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    UtilityKeybinds();
}

void Nullity::Engine::Render(Camera& camera)
{
    data.objectShader.use();
    data.objectShader.setVec3("viewPos", camera.position);

    TextureManager::Get().SendSubTexResArrayToShader(data.texArrayDataUBO); // send the tex res array to the frag shader

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)state.viewRes.x / state.viewRes.y, 0.1f, 1000.0f);

    SetUniformBufferData(data.matricesUBO, 0, 64, glm::value_ptr(view));
    SetUniformBufferData(data.matricesUBO, 64, 64, glm::value_ptr(projection));
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, data.skyboxCubemap); // binding skybox for reflections

    WorldObjectSystem(data.registry);
    DrawSystem(data.registry);

    // skybox
    glDepthFunc(GL_LEQUAL);
    data.skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

    SetUniformBufferData(data.matricesUBO, 0, 64, glm::value_ptr(view));

    glBindVertexArray(data.skyboxVAO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, data.skyboxCubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    glBindVertexArray(0);


    framebuffer.Unbind();
}

void Nullity::Engine::RenderFramebuffer()
{
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(state.viewOffset.x, state.viewOffset.y, state.viewRes.x, state.viewRes.y);

    data.renderTexShader.use();
    glBindVertexArray(renderTexVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, framebuffer.GetColorTexture());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Nullity::Engine::ExitFrame()
{
    keysRefresh();
    glfwSwapBuffers(window);
    glfwPollEvents();
}


bool GlfwOpenGLInit(GLFWwindow*& window, Nullity::Engine& eng)
{
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // remove when testing performance and shipping 
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(eng.state.initViewRes.x, eng.state.initViewRes.y, "Nullity Test", NULL, NULL);
    if (window == NULL) {
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

    glfwSetWindowUserPointer(window, &eng);

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

bool Nullity::Engine::Init()
{
    if (!GlfwOpenGLInit(window, *this))
        return false;

    data.InitData();

    reloadConfigKeymaps();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    TexturesInit(data);

    return true;
}
