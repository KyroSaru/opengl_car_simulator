#version 420 core

// Positions
layout (location = 0) in vec3 aPos;
// Coord. des normales
layout (location = 1) in vec3 aNormal;
// Coord. de texture
layout (location = 2) in vec2 aTexCoords;

// En sortie pour le Frag Shader (position, normal, coord. de texture du fragment)
out vec3 fragPos;
out flat vec3 Normal;
out vec2 texCoords;

// Matrice modèle, vue et projection des modèles
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	// On multiplie par la matrice inverse transposée pour éviter les déformations des normales (ex: rotation, homothétie)
	Normal = mat3(transpose(inverse(model))) * aNormal;
	texCoords = aTexCoords;
	
	// Position de tous les vertices (en prenant en compte les transformations et la caméra)
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
