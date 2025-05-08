#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    // Avec z=w, on a une profondeur de 1.0f
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    // Changer l'axe des z à cause des systèmes de coord. diff. (main gauche, main droite)
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}    