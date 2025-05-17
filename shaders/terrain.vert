#version 420 core

// Positions
layout (location = 0) in vec3 aPos;
// Coord. des normales
layout (location = 1) in vec3 aNormal;

// En sortie pour le Frag Shader (position, normal, coord. de texture du fragment)
out vec3 fragPos;
out flat vec3 Normal;

// Matrice mod�le, vue et projection des mod�les
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	// On multiplie par la matrice inverse transpos�e pour �viter les d�formations des normales (ex: rotation, homoth�tie)
	Normal = mat3(transpose(inverse(model))) * aNormal;
	
	// Position de tous les vertices (en prenant en compte les transformations et la cam�ra)
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
