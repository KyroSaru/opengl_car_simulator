#version 420 core

// Positions
layout (location = 0) in vec3 aPos;

// Coord. de textures pour le Frag Shader
out vec3 texCoords;

// Matrice de projection et de vue
uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Changer l'axe des z à cause des systèmes de coord. diff. (main gauche, main droite)
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);

    vec4 position = projection * view * vec4(aPos, 1.0f);
    // Avec z=w, on a une profondeur de 1.0f
    gl_Position = vec4(position.x, position.y, position.w, position.w);
}
