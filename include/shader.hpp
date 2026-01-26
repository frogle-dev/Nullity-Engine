#pragma once

#include "glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


void SetUniformBufferData(GLuint ubo, unsigned int offset, unsigned int size, const void* data);


class Shader
{
public:
    // the shader program ID
    unsigned int shaderProgramID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader program
    void use();
    // delete shader program
    void deleteProgram();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setIVec2(const std::string &name, int value1, int value2) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec2(const std::string &name, glm::vec2 value) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};