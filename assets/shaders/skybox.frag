#version 460 core

out vec4 FragColor;

in VS_OUT
{
    vec3 texCoords;
} fs_in;

layout (binding = 1) uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, fs_in.texCoords);
}