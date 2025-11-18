#version 460 core

out vec4 FragColor;

in vec3 color;
in vec3 texCoord;

uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, texCoord) * vec4(color, 1.0);
}