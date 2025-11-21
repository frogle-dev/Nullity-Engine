#version 460 core

out vec4 FragColor;

in vec3 color;
in vec3 texCoord;

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform samplerCube cubemap;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;

    FragColor = texture(cubemap, texCoord) * vec4(result, 1.0);
}