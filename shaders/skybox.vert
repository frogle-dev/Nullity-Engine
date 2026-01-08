#version 460 core

layout (location = 0) in vec3 aPos;

out VS_OUT
{
    vec3 texCoords;
} vs_out;

layout (std140, binding = 0) uniform Matrices
{
    mat4 view; // 64 bytes
    mat4 projection; // 64 bytes
};

void main()
{
    vs_out.texCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}