#version 420 core

layout (location = 0) in vec3 aPos;       // Position du vertex
layout (location = 1) in vec3 aNormal;    // Normale du vertex
layout (location = 2) in vec2 aTexCoords; // Coord. de texture

uniform mat4 model;       // Matrice de transfo. du modèle
uniform mat4 view;        // Matrice de vue
uniform mat4 projection;  // Matrice de proj.

out vec3 FragPos;         // Position du fragment dans l'espace monde
out flat vec3 Normal;          // Normale interpolée
out vec2 TexCoords;       // Coord. de texture

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transforme la normale pour éviter les déformations (ex: rotation, homothétie)
    TexCoords = aTexCoords; // Transmet les coord. de texture

    gl_Position = projection * view * vec4(FragPos, 1.0); // Pos. finale
}