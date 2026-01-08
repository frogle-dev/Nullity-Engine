#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;


out VS_OUT 
{
    vec2 texCoord;
} vs_out;

uniform mat4 positions[100];

layout (std140, binding = 0) uniform Matrices
{
    mat4 view; // 64 bytes
    mat4 projection; // 64 bytes
};


void main()
{
    mat4 model = positions[gl_InstanceID];

    gl_Position = projection * view * model * vec4(inPos, 1.0);
    
    vs_out.texCoord = inTexCoord;
}