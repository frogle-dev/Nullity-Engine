#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "models.hpp"
#include "debugging.hpp"
#include "shader.hpp"
#include "textures.hpp"

#include <vector>
#include <string>

using namespace Nullity;


Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
    : vertices(vertices), indices(indices), textures(textures)
{
    SetupMesh();
}

Mesh::Mesh(unsigned int instanceCount, std::vector<glm::mat4>& instanceMatrices, 
    std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
    : instanceCount(instanceCount), instanceMatrices(instanceMatrices), vertices(vertices), indices(indices), textures(textures)
{
    SetupMeshInstanced();
}


void Mesh::Draw(Shader &shader)
{
    shader.use();

    GLuint texArrayID = textureManager.GetTexArrayID();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayID);

    int numDiffuse = 0;
    int numSpecular = 0;
    int numEmission = 0;
    for (int i = 0; i < textures.size(); i++)
    {
        unsigned int curLayer = textures[i].layer;

        switch (textures[i].type)
        {
            case TextureType::DIFFUSE:
            {
                if (numDiffuse == 0)
                {
                    shader.setInt("material.diffuseStartLayer", curLayer); 
                }
                numDiffuse++;
                break;
            }
            case TextureType::SPECULAR:
            {
                if (numSpecular == 0)
                {
                    shader.setInt("material.specularStartLayer", curLayer); 
                }
                numSpecular++;
                break;
            }
            case TextureType::EMISSION:
            {
                if (numEmission == 0)
                {
                    shader.setInt("material.emissionStartLayer", curLayer); 
                }
                numEmission++;
                break;
            }
        }
    }

    if (numDiffuse)
        shader.setInt("material.diffuseLayerCount", numDiffuse);
    if (numSpecular)
        shader.setInt("material.specularLayerCount", numSpecular);
    if (numEmission)
        shader.setInt("material.emissionLayerCount", numEmission);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::DrawInstanced(Shader &shader)
{
    shader.use();

    if (textures.size() > 0)
    {
        GLuint texArrayID = textureManager.GetTexArrayID();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayID);

        shader.setInt("layer", textures[0].layer);
    }
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0, instanceCount);

    glBindVertexArray(0);
}


void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texure coord attributre
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::SetupMeshInstanced()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texure coord attributre
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    // use texture or use material for vertex
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, useTex));

    GLuint matrixBuffer; // instance transformation matrices
    glGenBuffers(1, &matrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::mat4), instanceMatrices.data(), GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);

    glBindVertexArray(0);
}




Model::Model(std::string path)
{
    LoadModel(path);
}

Model::Model(std::string path, unsigned int instanceCount, std::vector<glm::mat4>& instanceMatrices) 
    : instanceCount(instanceCount), instanceMatrices(instanceMatrices)
{
    LoadModel(path);
}

void Model::Draw(Shader &shader)
{
    if (instanceCount == 0)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].Draw(shader);
        }
    }
    else
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].DrawInstanced(shader);
        }
    }
}


void Model::LoadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::ostringstream oss;
        oss << "(Assimp): Error: " << importer.GetErrorString() << std::endl;

        debug.Log(oss);
        return;
    }
    directory = path.substr(0, path.find_last_of('/')); //get the directory the model is in

    ProcessNode(scene->mRootNode, scene); //recxursive functrion process all the nodes and processes the meshes within each node, starts at root node
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    // process all the meshes in current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    // recursively process all meshes in each child node
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // initializing vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector3;
        vector3.x = mesh->mVertices[i].x; // mVertices is the vertex position array kinda weird name
        vector3.y = mesh->mVertices[i].y;
        vector3.z = mesh->mVertices[i].z;
        vertex.position = vector3;

        vector3.x = mesh->mNormals[i].x;
        vector3.y = mesh->mNormals[i].y;
        vector3.z = mesh->mNormals[i].z;
        vertex.normal = vector3;

        if (mesh->mTextureCoords[0]) // checking if the mesh actually contains any tex coords
        {
            glm::vec2 vector2;
            vector2.x = mesh->mTextureCoords[0][i].x;
            vector2.y = mesh->mTextureCoords[0][i].y;

            vertex.texCoords = vector2;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        if (scene->mNumMaterials > mesh->mMaterialIndex) // use textures or material color
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor4D diffuse;
            
            if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
            {
                vertex.color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
            }

            vertex.useTex = material->GetTextureCount(aiTextureType_DIFFUSE) > 0;
        }

        vertices.push_back(vertex);
    }

    // initializing indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j <face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    
    if (mesh->mMaterialIndex >= 0) // check if mesh contains tex materials
    {        
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); // insert the entire diffuseMaps vector at the end of texture vector

        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); // likewise as before
    }

    if (instanceCount == 0)
        return Mesh(vertices, indices, textures);
    else
        return Mesh(instanceCount, instanceMatrices, vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType internalType)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        { // load texture if it hasnt already been loaded
            Texture texture;
            texture.layer = textureManager.LoadTextureIntoTexArray(str.C_Str(), directory);
            texture.type = internalType;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}
