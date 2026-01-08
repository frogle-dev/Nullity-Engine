#version 460 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices
{
    mat4 view; // 64 bytes
    mat4 projection; // 64 bytes
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}