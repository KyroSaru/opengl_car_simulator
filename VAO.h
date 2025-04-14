#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	// ID du VAO
	GLuint ID;
	// Constructeur générant un ID de VAO
	VAO();

	// Lie les attributs du VBO au VAO en utilisant un layout spécifié
	void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// Lie le VAO
	void Bind();
	// Délie le VAO
	void Unbind();
	// Supprime le VAO
	void Delete();
};

#endif // VAO_H