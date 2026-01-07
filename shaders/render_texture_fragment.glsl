#version 460 core

out vec4 FragColor;

in VS_OUT
{
    vec2 texCoords;
} fs_in;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, fs_in.texCoords);
}