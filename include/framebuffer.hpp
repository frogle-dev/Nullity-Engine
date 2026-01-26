#pragma once

#include <iostream>

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class Framebuffer
{
public:
    Framebuffer(float width, float height);
    void Cleanup();
    GLuint& GetColorTexture();
    void Rescale(float width, float height);
    void Bind();
    void Unbind();

private:
    void GenerateFrameBufferData(float width, float height);

    GLuint fbo;
    GLuint colTex;
    GLuint rbo;
};