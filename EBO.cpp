#include "EBO.h"

// Constructeur g�n�rant un Element Buffer et le liant aux indices
EBO::EBO(std::vector<GLuint>& indices)
{
	// G�n�re un EBO avec un ID unique
	glGenBuffers(1, &ID);
	// Lie le EBO pour en faire le EBO courant en sp�cifiant qu'il est de type GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Copie les donn�es des indices dans le EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

// Lie le EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// D�lie le EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Supprime le EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}