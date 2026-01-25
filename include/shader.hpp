#pragma once

#include "glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


void SetUniformBufferData(GLuint ubo, unsigned int offset, unsigned int size, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


class Shader
{
public:
    // the shader program ID
    unsigned int shaderProgramID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath)
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
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vertexShaderCode = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();

        // 2) compile shaders
        unsigned int vertex, fragment;

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
    void use()
    {
        glUseProgram(shaderProgramID);
    }

    // delete shader program
    void deleteProgram()
    {
        glDeleteProgram(shaderProgramID);
    }
    // utility uniform functions
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        int loc = glGetUniformLocation(shaderProgramID, name.c_str());
        glUniform1i(loc, value);

        if (loc == -1) std::cout << "(Shader): Error: Uniform not found " << name << std::endl;
    }
    void setIVec2(const std::string &name, int value1, int value2) const
    {
        int loc = glGetUniformLocation(shaderProgramID, name.c_str());

        if (loc == -1) std::cout << "(Shader): Error: Uniform not found " << name << std::endl;

        glUniform2i(loc, value1, value2);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setMat3(const std::string &name, glm::mat3 value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec3(const std::string &name, glm::vec3 value) const
    {
        glUniform3f(glGetUniformLocation(shaderProgramID, name.c_str()), value.x, value.y, value.z);
    }
    void setVec2(const std::string &name, glm::vec2 value) const
    {
        glUniform2f(glGetUniformLocation(shaderProgramID, name.c_str()), value.x, value.y);
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "Error: shader compilation error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "Error: program linking error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};