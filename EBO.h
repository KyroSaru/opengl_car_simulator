#ifndef EBO_H
#define EBO_h

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID du EBO
	GLuint ID;
	// Constructeur générant un Element Buffer et le liant aux indices
	EBO(std::vector<GLuint>& indices);

	// Lie le EBO
	void Bind();
	// Délie le EBO
	void Unbind();
	// Supprime le EBO
	void Delete();
};

#endif // EBO_H