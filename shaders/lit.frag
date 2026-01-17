#version 460 core

out vec4 FragColor;

in VS_OUT 
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
} fs_in;


layout (binding = 1) uniform samplerCube skybox;

layout (binding = 0) uniform sampler2DArray texArray;

layout (std140, binding = 1) uniform TexArrayData
{
    vec2 bucketSize;
    vec2 subTexRes[100]; // size should be same as tex array
};

uniform vec3 viewPos;


#define USE_FOG 0


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


struct DirLight
{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight 
{    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NUM_POINT_LIGHTS 1  
uniform PointLight pointLights[NUM_POINT_LIGHTS];

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;



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

float near = 0.1;
float far = 100.0;
float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // convert depth to normalized device coords
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 calcAmbient(vec3 lightAmbient);
vec3 calcDiffuse(vec3 lightDiffuse, float diffuseAmount);
vec3 calcSpecular(vec3 lightSpecular, float specularAmount);


void main()
{
    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    // directional lights
    vec3 result = vec3(0.0);
    result += calcDirLight(dirLight, norm, viewDir);

    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        result += calcPointLight(pointLights[i], norm, fs_in.fragPos, viewDir);
    }

    result += calcSpotLight(spotLight, norm, fs_in.fragPos, viewDir);

    // emission
    vec3 emission = vec3(0.0);
    if (material.emissionLayerCount > 0)
    {
        for (int i = 0; i < material.emissionLayerCount; i++)
        {
            emission += material.emissionStrength * sampleTexArraySubtex(material.emissionStartLayer + i).rgb;
        }
    }
    result += emission;

    // reflection
    vec3 I = normalize(fs_in.fragPos - viewPos);
    vec3 R = reflect(I, norm);
    vec4 reflectColor = vec4(texture(skybox, R).rgb, 1.0);

    //fog
    if (USE_FOG == 1)
    {
        float fogDensity = 5;
        float depth = linearizeDepth(gl_FragCoord.z) / far;
        float fogDepth = exp(-pow(depth * fogDensity, 2.0));
        vec3 fogColor = vec3(0.902, 0.902, 0.980);
        result = mix(fogColor, result, fogDepth);
    }

    FragColor = mix(vec4(result, 1.0), reflectColor, 0.1);
}


vec3 calcAmbient(vec3 lightAmbient)
{
    vec3 ambient = vec3(0.0);
    if (material.diffuseLayerCount > 0)
    {
        for (int i = 0; i < material.diffuseLayerCount; i++)
        {
            ambient += lightAmbient * sampleTexArraySubtex(material.diffuseStartLayer + i).rgb;
        }
    }

    return ambient;
}
vec3 calcDiffuse(vec3 lightDiffuse, float diffuseAmount)
{
    vec3 diffuse = vec3(0.0);
    if (material.diffuseLayerCount > 0)
    {
        for (int i = 0; i < material.diffuseLayerCount; i++)
        {
            diffuse += lightDiffuse * diffuseAmount * sampleTexArraySubtex(material.diffuseStartLayer + i).rgb;
        }
    }

    return diffuse;
}
vec3 calcSpecular(vec3 lightSpecular, float specularAmount)
{
    vec3 specular = vec3(0.0);
    if (material.specularLayerCount > 0)
    {
        for (int i = 0; i < material.specularLayerCount; i++)
        {
            specular += lightSpecular * specularAmount * sampleTexArraySubtex(material.specularStartLayer + i).rgb;
        }
    }

    return specular;
}


vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine diffuse specular and ambient
    vec3 result = calcAmbient(light.ambient) + calcDiffuse(light.diffuse, diff) + calcSpecular(light.specular, spec);
    return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine diffuse specular and ambient
    vec3 ambient = calcAmbient(light.ambient);
    vec3 diffuse = calcDiffuse(light.diffuse, diff);
    vec3 specular = calcSpecular(light.specular, spec);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);


    // angle between where the spotlight is pointing and the fragment
    float theta = dot(lightDir, normalize(-light.direction));
    // values for smooth cutoff at the edges
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = calcAmbient(light.ambient);
    vec3 diffuse = calcDiffuse(light.diffuse, diff);
    vec3 specular = calcSpecular(light.specular, spec);
    diffuse *= intensity;
    specular *= intensity;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);  
}