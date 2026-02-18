#pragma once


#include "glad.h"
#include <GLFW/glfw3.h>


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
