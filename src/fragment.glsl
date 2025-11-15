#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, texCoord) * vec4(color, 1.0);
}