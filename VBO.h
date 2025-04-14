#ifndef VBO_H
#define VBO_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

// Structure pour "unifier" les vertices dans les Meshs
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class VBO
{
public:
	// ID du VBO
	GLuint ID;
	// Constructeur générant un Vertex Buffer et le liant aux vertices
	VBO(std::vector<Vertex>& vertices);

	// Lie le VBO
	void Bind();
	// Délie le VBO
	void Unbind();
	// Supprime le VBO
	void Delete();
};

#endif // VBO_H