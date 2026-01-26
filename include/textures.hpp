#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include "shader.hpp"

#include <iostream>


// singleton
class TextureManager
{
public:
    static TextureManager& Get();
    
    GLuint GetTexArrayID() const;

    auto& GetTexSubTexResArray() const;

    void GenerateTextureArray(int _maxTexWidth, int _maxTexHeight, int _maxTextures, GLuint ubo);

    GLuint LoadStandaloneTexture(std::string path);

    int LoadTextureIntoTexArray(std::string path, std::string directoryPath);

    GLuint LoadCubemap(std::vector<std::string> faces);

    void GenerateMipmaps();

    void SendSubTexResArrayToShader(GLuint ubo);

private:
    // private constructor so other instances cant be made
    TextureManager();

    GLuint texArrayID;
    std::vector<glm::vec2> subTexRes;
    int maxTexWidth;
    int maxTexHeight;
    
    int mipLevels;

    int maxTexLayers;
    int nextTexLayer;
};
