#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	// ID du VAO
	GLuint ID;
	// Constructeur g�n�rant un ID de VAO
	VAO();

	// Lie les attributs du VBO au VAO en utilisant un layout sp�cifi�
	void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// Lie le VAO
	void Bind();
	// D�lie le VAO
	void Unbind();
	// Supprime le VAO
	void Delete();
};

#endif // VAO_H