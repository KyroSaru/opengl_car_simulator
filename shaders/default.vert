#version 420 core

// Positions
layout (location = 0) in vec3 aPos;
// Coordonn�es des normales
layout (location = 1) in vec3 aNormal;
// Coordonn�es de texture
layout (location = 2) in vec2 aTexCoords;

// En sortie pour le Frag Shader
out vec3 FragPos;
out flat vec3 Normal;
out vec2 TexCoords;

// Importe la matrice de mod�le, vue et projection de l'objet depuis la fonction main
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	// On multiplie par la matrice inverse transpos�e pour �viter les d�formations des normales (ex: rotation, homoth�tie)
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
	
	// En sortie, les positions de tous les vertices (en prenant en compte la cam�ra)
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
