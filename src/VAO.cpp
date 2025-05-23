#include "VAO.h"

// Constructeur générant un ID de VAO
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Lie les attributs du VBO au VAO en utilisant un layout spécifié
void VAO::LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	vbo.Bind();
	// Configure l'interprétation des données (attributs des vertices) du VBO dans le VAO
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, (GLsizei)stride, offset);
	// Active l'attribut 0 du VAO pour qu'OpenGL puisse l'utiliser
	glEnableVertexAttribArray(layout);
	vbo.Unbind();

}

// Lie le VAO
void VAO::Bind()
{ 
	glBindVertexArray(ID);
}

// Délie le VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Supprime le VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}
