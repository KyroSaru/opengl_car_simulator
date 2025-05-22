#version 420 core

in float vAlpha;

uniform vec3 trailColor;
uniform float lightIntensity;

out vec4 FragColor;

void main()
{
    FragColor = vec4(trailColor * lightIntensity, vAlpha);
}