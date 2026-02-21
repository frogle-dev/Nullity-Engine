#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>


namespace Nullity
{
    class Framebuffer
    {
    public:
        Framebuffer() = default;
        Framebuffer(float width, float height);
        void Cleanup();
        GLuint& GetColorTexture();
        void Refresh(float width, float height);
        void Bind();
        void Unbind();

    private:
        GLuint fbo;
        GLuint colTex;
        GLuint rbo;
    };
}
