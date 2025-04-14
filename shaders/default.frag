#version 420 core

out vec4 FragColor;

// Variable récupérée depuis le Vertex Shader (import des coord de textures, des normales et position du fragment)
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

// Obtenir le texture unit
uniform sampler2D diffuse0;

void main()
{
	// Sortie de la couleur finale
	FragColor = texture(diffuse0, texCoords);
}