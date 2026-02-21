#include "debugging.hpp"
#include "framebuffer.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>


Nullity::Framebuffer::Framebuffer(float width, float height)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colTex);
    glGenRenderbuffers(1, &rbo);

    Refresh(width, height);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::ostringstream oss;
        oss << "(Framebuffer): Error: Framebuffer is not complete" << std::endl;
        debug.Log(oss);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Nullity::Framebuffer::Cleanup()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &colTex);
    glDeleteRenderbuffers(1, &rbo);
}

GLuint& Nullity::Framebuffer::GetColorTexture()
{
    return colTex;
}

void Nullity::Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Nullity::Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Nullity::Framebuffer::Refresh(float width, float height)
{
    Bind();

    // texture assignment to framebuffer
    glBindTexture(GL_TEXTURE_2D, colTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colTex, 0);

    // rbo (depth and/or stencil buffer) assignment to framebuffer 
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    Unbind();
}
