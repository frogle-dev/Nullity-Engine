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

vec4 calcDiffuse()
{
    vec4 diffuse = vec4(0.0);
    if (material.diffuseLayerCount > 0)
    {
        for (int i = 0; i < material.diffuseLayerCount; i++)
        {
            diffuse += sampleTexArraySubtex(material.diffuseStartLayer + i);
        }
    }

    return diffuse;
}

void main()
{
    FragColor = calcDiffuse();
}