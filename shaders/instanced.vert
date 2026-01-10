#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in mat4 instanceMatrix;


out VS_OUT 
{
    vec2 texCoord;
} vs_out;


layout (std140, binding = 0) uniform Matrices
{
    mat4 view; // 64 bytes
    mat4 projection; // 64 bytes
};


void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(inPos, 1.0);
    
    vs_out.texCoord = inTexCoord;
}