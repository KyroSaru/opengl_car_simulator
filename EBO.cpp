#include "EBO.h"

// Constructeur générant un Element Buffer et le liant aux indices
EBO::EBO(std::vector<GLuint>& indices)
{
	// Génère un EBO avec un ID unique
	glGenBuffers(1, &ID);
	// Lie le EBO pour en faire le EBO courant en spécifiant qu'il est de type GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Copie les données des indices dans le EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

// Lie le EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Délie le EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Supprime le EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}