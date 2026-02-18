#version 460 core

out vec4 FragColor;

layout (binding = 0) uniform sampler2DArray texArray;
uniform int layer;

layout (std140, binding = 1) uniform TexArrayData
{
    vec2 bucketSize;
    vec2 subTexRes[100]; // size should be same as tex array
};

in VS_OUT 
{
    vec2 texCoord;
    vec4 color;
    float useTex;
} fs_in;

vec4 sampleTexArraySubtex(int layer)
{
    vec2 subSize = subTexRes[layer];

    vec2 wrapped = fract(fs_in.texCoord);
    vec2 transformed = wrapped * (subSize / bucketSize);

    vec4 textureColor = texture(texArray, vec3(transformed, float(layer)));
    if(textureColor.a < 0.1)
    {
        discard;
    }
    return textureColor;
}


void main()
{
    if (fs_in.useTex > 0.5)
    {
        FragColor = sampleTexArraySubtex(layer);
    }
    else
    {
        FragColor = fs_in.color;
    }
}
