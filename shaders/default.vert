#version 420 core

// Positions
layout (location = 0) in vec3 aPos;
// Coordonnées des normales
layout (location = 1) in vec3 aNormal;
// Coordonnées de texture
layout (location = 2) in vec2 aTexCoords;

// En sortie pour le Frag Shader
out vec3 FragPos;
out flat vec3 Normal;
out vec2 TexCoords;

// Importe la matrice de modèle, vue et projection de l'objet depuis la fonction main
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	// On multiplie par la matrice inverse transposée pour éviter les déformations des normales (ex: rotation, homothétie)
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
	
	// En sortie, les positions de tous les vertices (en prenant en compte la caméra)
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
