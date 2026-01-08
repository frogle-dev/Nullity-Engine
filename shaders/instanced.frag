#version 460 core

out vec4 FragColor;

layout (binding = 5) uniform sampler2D tex;

in VS_OUT 
{
    vec2 texCoord;
} fs_in;


void main()
{
    FragColor = texture(tex, fs_in.texCoord);
}


