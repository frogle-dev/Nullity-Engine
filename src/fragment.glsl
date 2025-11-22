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
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

    // specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // caculate specular
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}