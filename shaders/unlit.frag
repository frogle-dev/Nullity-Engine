#version 460 core

out vec4 FragColor;

in VS_OUT 
{
    vec2 texCoord;
} fs_in;


layout (binding = 0) uniform sampler2DArray texArray;

layout (std140, binding = 1) uniform TexArrayData
{
    vec2 bucketSize;
    vec2 subTexRes[100]; // size should be same as tex array
};

struct Material
{
    int diffuseLayerCount;
    int diffuseStartLayer;
    int specularLayerCount;
    int specularStartLayer;
    int emissionLayerCount;
    int emissionStartLayer;

    float emissionStrength;
    float shininess;
};
uniform Material material;


vec4 sampleTexArraySubtex(int layer)
{
    vec2 subSize = subTexRes[layer];

    vec2 wrapped = fract(fs_in.texCoord);
    vec2 transformed = wrapped * (subSize / bucketSize);

    vec4 textureColor = texture(texArray, vec3(transformed, float(layer)));
    if(textureColor.a < 0.5)
    {
        discard;
    }
    return textureColor;
}


void main()
{
    FragColor = sampleTexArraySubtex(material.diffuseStartLayer);
}