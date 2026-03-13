#include "core.hpp"
#include "debugging.hpp"
#include "textures.hpp"
#include "systems.hpp"
#include "render.hpp"
#include "primitives.hpp"
#include "input.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace N = Nullity;

void N::EngineClose()
{
    Data::Cleanup();
    framebuffer.Cleanup();

    glfwTerminate();
}

bool N::Running()
{
    return !glfwWindowShouldClose(window);
}

void N::UtilityKeybinds()
{
    if (Input::IsActionJustPressed("focus"))
    {
        focused = !focused;
    }
}

void N::EnterFrame()
{
    if (focused)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float currentFrame = glfwGetTime();
    Time::deltaTime = currentFrame - Time::lastFrame;
    Time::lastFrame = currentFrame;
    
    Time::msPerFrame = Time::deltaTime * 1000;
    Time::fps = 1000 / Time::msPerFrame;


    framebuffer.Bind();
    glViewport(0,0, viewRes.x, viewRes.y);
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    UtilityKeybinds();
}

void N::Render(Camera& camera)
{
    Data::objectShader.use();
    Data::objectShader.setVec3("viewPos", camera.position);

    textureManager.SendSubTexResArrayToShader(Data::texArrayDataUBO); // send the tex res array to the frag shader

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), (float)viewRes.x / viewRes.y, 0.1f, 1000.0f);

    SetUniformBufferData(Data::matricesUBO, 0, 64, glm::value_ptr(view));
    SetUniformBufferData(Data::matricesUBO, 64, 64, glm::value_ptr(projection));
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Data::skyboxCubemap); // binding skybox for reflections

    WorldObjectSystem(registry);
    DrawSystem(registry);

    // skybox
    glDepthFunc(GL_LEQUAL);
    Data::skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

    SetUniformBufferData(Data::matricesUBO, 0, 64, glm::value_ptr(view));

    glBindVertexArray(Data::skyboxVAO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Data::skyboxCubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    glBindVertexArray(0);


    framebuffer.Unbind();
}

void N::RenderFramebuffer()
{
    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(viewOffset.x, viewOffset.y, viewRes.x, viewRes.y);

    Data::renderTexShader.use();
    glBindVertexArray(renderTexVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, framebuffer.GetColorTexture());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void N::ExitFrame()
{
    Input::KeysRefresh();
    glfwSwapBuffers(window);
    glfwPollEvents();
}


bool GlfwOpenGLInit(GLFWwindow*& window)
{
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // remove when testing performance and shipping 
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(N::initViewRes.x, N::initViewRes.y, "Nullity Test", NULL, NULL);
    if (window == NULL) {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to create GLFW window" << std::endl; 
        N::Debug::Log(oss);

        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::ostringstream oss;
        oss << "(Initialization): Error: Failed to initialize GLAD" << std::endl;
        N::Debug::Log(oss);
        return false;
    }

    N::Debug::DebugInit();

    glfwSetWindowSizeCallback(window, N::WindowSizeCallback); // NOTE: try framebuffer size callback
    glfwSetCursorPosCallback(window, N::Input::MouseCallback);
    glfwSetKeyCallback(window, N::Input::KeyCallback);

    return true;
}

bool TexturesInit()
{
    glActiveTexture(GL_TEXTURE0);

    N::textureManager.GenerateTextureArray(4096, 4096, 100, N::Data::texArrayDataUBO);
    
    GLuint texArrayID = N::textureManager.GetTexArrayID();

    N::Data::objectShader.use();
    N::Data::objectShader.setFloat("material.emissionStrength", 1.0f);
    N::Data::objectShader.setFloat("material.shininess", 128.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    N::textureManager.GenerateMipmaps();
    N::textureManager.SendSubTexResArrayToShader(N::Data::texArrayDataUBO);

    return true;
}

bool Init()
{
    if (!GlfwOpenGLInit(N::window))
        return false;

    N::Data::InitData();

    N::Input::InputInit();
    N::Input::ReloadConfigKeymaps();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    TexturesInit();

    return true;
}

void N::EngineInit()
{
    if(!Init())
	{
        std::ostringstream oss;
        oss << "Engine failed to initialize" << std::endl;
        Debug::Log(oss);
	}

    framebuffer = Framebuffer(viewRes.x, viewRes.y);

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


void N::Data::InitData()
{
    InitShaders();
    InitSkybox();
    InitUBOs();
}

void N::Data::InitShaders()
{
    objectShader = Shader("assets/shaders/lit.vert", "assets/shaders/lit.frag");
    lightSourceShader = Shader("assets/shaders/light_source.vert", "assets/shaders/light_source.frag");
    skyboxShader = Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    instancedShader = Shader("assets/shaders/instanced.vert", "assets/shaders/instanced.frag"); 
    grassShader = Shader("assets/shaders/grass.vert", "assets/shaders/grass.frag");
    unlitShader = Shader("assets/shaders/unlit.vert", "assets/shaders/unlit.frag");
    renderTexShader = Shader("assets/shaders/render_texture.vert","assets/shaders/render_texture.frag");
}

void N::Data::InitUBOs()
{
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glGenBuffers(1, &texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, texArrayDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, 1616, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, texArrayDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void N::Data::InitSkybox()
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> skyboxFaces = {
        "assets/images/skybox/right.jpg",
        "assets/images/skybox/left.jpg",
        "assets/images/skybox/top.jpg",
        "assets/images/skybox/bottom.jpg",
        "assets/images/skybox/front.jpg",
        "assets/images/skybox/back.jpg",
    };
    skyboxCubemap = textureManager.LoadCubemap(skyboxFaces);
}

void N::Data::Cleanup()
{
    glDeleteBuffers(1, &matricesUBO);
    glDeleteBuffers(1, &texArrayDataUBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();
    skyboxShader.deleteProgram();
    instancedShader.deleteProgram();
    grassShader.deleteProgram();
    unlitShader.deleteProgram();
}


void N::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    // letterbox scaling
    float aspect = (float)width / height;
    float targetAspect = (float)initViewRes.x / (float)initViewRes.y;

    if (aspect > targetAspect)
    {
        viewRes.y = height;
        viewRes.x = (int)(height * targetAspect);
    }
    else
    {
        viewRes.x = width;
        viewRes.y = (int)(width / targetAspect);
    }

    viewOffset.x = (width - viewRes.x) / 2;
    viewOffset.y = (height - viewRes.y) / 2;

    glViewport(viewOffset.x, viewOffset.y, viewRes.x, viewRes.y);

    framebuffer.Refresh(viewRes.x, viewRes.y);
}
