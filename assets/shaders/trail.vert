#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aAlpha;

uniform mat4 view;
uniform mat4 projection;

out float vAlpha;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    vAlpha = aAlpha;
}