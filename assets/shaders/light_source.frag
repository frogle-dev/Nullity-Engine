#version 460 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(gl_FragCoord.xy / 1000.0, (gl_FragCoord.x * gl_FragCoord.y) / 1000.0, 1.0); // set all 4 vector values to 1.0
}