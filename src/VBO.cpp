#include "VBO.h"

// Constructeur g�n�rant un Vertex Buffer et le liant aux vertices
VBO::VBO(std::vector<Vertex>& vertices)
{
	// G�n�re un VBO avec un ID unique
	glGenBuffers(1, &ID);
	// Lie le VBO pour en faire le VBO courant en sp�cifiant qu'elle est de type GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	// Copie les donn�es des vertices dans le VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

// Lie le VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// D�lie le VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Supprime le VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}