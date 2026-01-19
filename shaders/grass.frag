#version 460 core

out vec4 FragColor;

in VS_OUT 
{
    vec2 texCoord;
} fs_in;

void main()
{
    vec2 st = gl_FragCoord.xy/vec2(1000,1000);

    vec4 startColor = vec4(0.53, 0.98, 0.6, 1.0);
    vec4 endColor = vec4(0.04, 0.54, 0.06, 1.0);

    float mixVal = distance(fs_in.texCoord, vec2(0,1));
    vec4 color = mix(endColor, startColor, mixVal);

    FragColor = color.xyzw;
}
