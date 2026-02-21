
#include "glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.hpp"
#include "debugging.hpp"

#include <string>
#include <fstream>
#include <iostream>


void Nullity::SetUniformBufferData(GLuint ubo, unsigned int offset, unsigned int size, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Nullity::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1) retrieve vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    // ensure ifstream objects can throw exceptions:
    vertexShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertexShaderStream, fragmentShaderStream;
        // read file's buffer contents into streams
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vertexShaderStream.str();
        fragmentCode = fragmentShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::ostringstream oss;
        oss << "(Shader): Error: File not succesfully read" << std::endl;

        debug.Log(oss);
    }
    const char* vertexShaderCode = vertexCode.c_str();
    const char* fragmentShaderCode = fragmentCode.c_str();

    // 2) compile shaders
    GLuint vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment shader'
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader program
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertex);
    glAttachShader(shaderProgramID, fragment);
    glLinkProgram(shaderProgramID);
    checkCompileErrors(shaderProgramID, "PROGRAM");

    // delete shaders once they are linked into shader program and just take up space :D
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
// use/activate the shader program
void Nullity::Shader::use()
{
    glUseProgram(shaderProgramID);
}

// delete shader program
void Nullity::Shader::deleteProgram()
{
    glDeleteProgram(shaderProgramID);
}
// utility uniform functions
void Nullity::Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(getUniformLoc(name), value);
}
void Nullity::Shader::setIVec2(const std::string &name, int value1, int value2) const
{
    glUniform2i(getUniformLoc(name), value1, value2);
}
void Nullity::Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(getUniformLoc(name), value);
}
void Nullity::Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(getUniformLoc(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Nullity::Shader::setMat3(const std::string &name, glm::mat3 value) const
{
    glUniformMatrix3fv(getUniformLoc(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Nullity::Shader::setVec3(const std::string &name, glm::vec3 value) const
{
    glUniform3f(getUniformLoc(name), value.x, value.y, value.z);
}
void Nullity::Shader::setVec2(const std::string &name, glm::vec2 value) const
{
    glUniform2f(getUniformLoc(name), value.x, value.y);
}


GLint Nullity::Shader::getUniformLoc(const std::string& name) const
{
    GLint loc = glGetUniformLocation(shaderProgramID, name.c_str());

    std::ostringstream oss;
    if (loc == -1)
    {
        oss << "(Shader): Error: uniform not found " << name << std::endl;
        debug.Log(oss);
    } 

    return loc;
}

void Nullity::Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    std::ostringstream oss;
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);

            oss << "Error: shader compilation error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

            debug.Log(oss);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);

            oss << "Error: program linking error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

            debug.Log(oss);
        }
    }
}
