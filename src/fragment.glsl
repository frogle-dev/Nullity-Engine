#version 460 core

out vec4 FragColor;

uniform samplerCube cubemap;
in vec3 texCoord;

in vec3 fragPos;


in vec3 normal;

struct Material
{
    samplerCube diffuse;
    samplerCube specular;
    samplerCube emission;
    float emissionStrength;
    float shininess;
};

struct Light 
{
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;

uniform Light light;
uniform Material material;



void main()
{
    // ambient lighting
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;

    // specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // caculate specular
    vec3 specular = light.specular * spec * texture(material.specular, texCoord).rgb;

    vec3 emission = texture(material.emission, texCoord).rgb * material.emissionStrength;

    vec3 result = ambient + diffuse + specular + emission;

    FragColor = vec4(result, 1.0);
}