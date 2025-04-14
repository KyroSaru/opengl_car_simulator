#ifndef EBO_H
#define EBO_h

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID du EBO
	GLuint ID;
	// Constructeur g�n�rant un Element Buffer et le liant aux indices
	EBO(std::vector<GLuint>& indices);

	// Lie le EBO
	void Bind();
	// D�lie le EBO
	void Unbind();
	// Supprime le EBO
	void Delete();
};

#endif // EBO_H