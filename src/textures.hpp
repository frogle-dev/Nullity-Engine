#pragma once

#include "../lib/glad.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include "shader.hpp"

#include <iostream>


// singleton
class TextureManager
{
public:
    static TextureManager& Get()
    {
        static TextureManager instance;
        return instance;
    }

    GLuint GetTexArrayID() const 
    {
        return texArrayID; 
    }

    auto& GetTexSubTexResArray() const
    {
        return subTexRes;
    }

    void GenerateTextureArray(int _maxTexWidth, int _maxTexHeight, int _maxTextures, GLuint ubo)
    {
        maxTexLayers = _maxTextures;
        maxTexWidth = _maxTexWidth;
        maxTexHeight = _maxTexHeight;

        SetUniformBufferData(ubo, 0, 8, glm::value_ptr(glm::vec2(maxTexWidth, maxTexHeight)));

        mipLevels = (int)std::floor(std::log2(maxTexWidth)) + 1;

        glGenTextures(1, &texArrayID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayID);
    
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
            mipLevels,                       // numbers of mipmaps
            GL_RGBA8,                        // color format
            maxTexWidth, maxTexHeight,       // max width and height with border padding
            maxTexLayers                     // max num of textures stored in array
        );
        
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }


    GLuint LoadStandaloneTexture(std::string path)
    {
        int width, height, numChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

        if (!data)
        {
            std::cout << "(Texture Manager): Texture Error: Failed to load texture" << std::endl;
            return -1;
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    int LoadTextureIntoTexArray(std::string path, std::string directoryPath)
    {
        if (nextTexLayer >= maxTexLayers)
        {
            std::cout << "(Texture Manager): Texture Array Error: cant have more textures than max specified for texture array" << std::endl;
            return -1;
        }


        int width, height, numChannels;
        unsigned char *data = stbi_load((directoryPath + "/" + path).c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

        if (!data)
        {
            std::cout << "(Texture Manager): Texture Array Error: Failed to load texture" << std::endl;
            return -1;
        }
        
        if (width > maxTexWidth || height > maxTexHeight)
        {
            std::cout << "(Texture Manager): Texture Array Error: width and height are larger than texture array width and height" << std::endl;
            return -1;
        }

        if (width != maxTexWidth || height != maxTexHeight)
        {
            std::cout << "(Texture Manager): Texture Array Warning: width and height do not match texture array width and height, "
            "texture will still be inserted but will not take up the full resolution." << std::endl;
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayID);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
            0,                          // mipmap level
            0, 0,                       // 1 pixel offset in order to create 1 pix border
            nextTexLayer,               // tex layer of tex array to insert image in
            width, height,              // size of image
            1,           
            GL_RGBA,                    // color format
            GL_UNSIGNED_BYTE, 
            data                        // pixel (image) data
        );

        subTexRes.push_back(glm::vec2(width, height));

        stbi_image_free(data);

        int texLayerUsed = nextTexLayer; // set to the current layer used for the texture just initalized
        nextTexLayer++;

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        return texLayerUsed; // return the layer just used in order to be used in shaders
    }


    GLuint LoadCubemap(std::vector<std::string> faces)
    {
        GLuint cubemap;
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        int width, height, numChannels;
        for (int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
            if (!data)
            {
                std::cout << "(Texture Manager): Texture Error: Failed to load cubemap" << std::endl;
                stbi_image_free(data);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return cubemap;
    }

    void GenerateMipmaps()
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayID);

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    void SendSubTexResArrayToShader(GLuint ubo)
    {
        std::vector<float> packed;
        packed.reserve(subTexRes.size() * 4);

        for (auto& res : subTexRes)
        {
            packed.push_back(res.x);
            packed.push_back(res.y);
            packed.push_back(0.0f);
            packed.push_back(0.0f);
        }

        SetUniformBufferData(ubo, 16, sizeof(float) * packed.size(), packed.data());
    }

private:
    // private constructor so other instances cant be made
    TextureManager() : texArrayID(0), maxTexWidth(0), maxTexHeight(0), maxTexLayers(0), nextTexLayer(0), mipLevels(0) {}

    GLuint texArrayID;
    std::vector<glm::vec2> subTexRes;
    int maxTexWidth;
    int maxTexHeight;
    
    int mipLevels;

    int maxTexLayers;
    int nextTexLayer;
};
