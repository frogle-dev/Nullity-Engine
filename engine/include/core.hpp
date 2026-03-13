#pragma once

#include "entt/entity/fwd.hpp"
#include "framebuffer.hpp"
#include "camera.hpp"
#include "shader.hpp"

#include <GLFW/glfw3.h>

#include <entt/entt.hpp>


namespace Nullity
{
	void EngineInit();
	void EngineClose();

    const GLFWwindow* GetWindow();
    const Framebuffer& GetFramebuffer();
    entt::registry& GetRegistry();
	bool Running();

	void EnterFrame();
	void Render(Camera& camera);
	void ExitFrame();
	void RenderFramebuffer();

	void UtilityKeybinds();

	void WindowSizeCallback(GLFWwindow* window, int width, int height);

	inline GLuint renderTexVAO;


	const glm::ivec2 initViewRes = glm::ivec2(1280, 720);
	inline glm::ivec2 viewRes = initViewRes;
	inline glm::ivec2 viewOffset = glm::ivec2(0, 0);

	inline bool wireframe = false;
	inline bool focused = true;


	namespace Time
	{
        inline float deltaTime = 0.0f;
        inline int fps;
        inline float msPerFrame;
        inline float lastFrame = 0.0f;
	}

    namespace Data
    {
        void InitData();

        inline Shader objectShader;
        inline Shader lightSourceShader;
        inline Shader skyboxShader;
        inline Shader instancedShader;
        inline Shader grassShader;
        inline Shader unlitShader;
        inline Shader renderTexShader;

        // uniform buffers
        inline GLuint matricesUBO;
        inline GLuint texArrayDataUBO;
        inline GLuint skyboxVAO, skyboxVBO;
        inline GLuint skyboxCubemap;

        void InitShaders();
        void InitUBOs();
        void InitSkybox();

        void Cleanup();
    }
}

