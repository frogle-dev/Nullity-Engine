#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"
#include "textures.hpp"

#include <iostream>


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec4 color;
    float useTex;
};

enum class TextureType
{
    DIFFUSE,
    SPECULAR,
    EMISSION
};

struct Texture
{
    unsigned int layer;
    TextureType type;
    std::string path;
};


class Mesh
{
public:
    unsigned int instanceCount = 0;
    std::vector<glm::mat4> instanceMatrices;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;


    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
 
    Mesh(unsigned int instanceCount, std::vector<glm::mat4>& instanceMatrices, 
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
    

    void Draw(Shader &shader);

    void DrawInstanced(Shader &shader);


private:
    // render buffers
    GLuint VAO, VBO, EBO; // vertex array object, vertex buffer object, element buffer object

    void SetupMesh();

    void SetupMeshInstanced();
};


class Model
{
public:

    Model(std::string path);

    Model(std::string path, unsigned int instanceCount, std::vector<glm::mat4>& instanceMatrices);
 

    void Draw(Shader &shader);


private:
    unsigned int instanceCount = 0;
    std::vector<glm::mat4> instanceMatrices;

    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<Texture> textures_loaded;


    void LoadModel(std::string path);

    void ProcessNode(aiNode *node, const aiScene *scene);

    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType internalType);
};
