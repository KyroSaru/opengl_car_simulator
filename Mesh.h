#ifndef MESH_H
#define MESH_h

#include <string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh {
public:
	// Mesh Data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Initialise le Mesh
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
	// Dessine le Mesh
	void Draw(Shader& shader);

private:
	// Stocke le VAO en privée pour l'utiliser dans la fonction Draw
	VAO VAO;
};

#endif // MESH_H