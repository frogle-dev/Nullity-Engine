#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout (std140, binding = 0) uniform Matrices
{
    mat4 view; // 64 bytes
    mat4 projection; // 64 bytes
};

void main()
{
    texCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}