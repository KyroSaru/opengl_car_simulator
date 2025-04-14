#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
	: vertices(vertices), indices(indices), textures(textures)
{
	// Lie le VAO
	VAO.Bind();
	// Génére un Vertex Buffer et le lie aux vertices
	VBO VBO(vertices);
	// Génére un Element Buffer et le lie aux indices
	EBO EBO(indices);
	// Lie les attributs du VBO (comme la position ou les couleurs) au VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // 0 = layout 0 (position des vertices)
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // 1 = layout 1 (position des normales)
	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // 2 = layout 2 (position des textures)
	// Délie tous les buffers pour éviter les conflits
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw(Shader& shader)
{
	shader.Activate();
	VAO.Bind();

	// Nombre de textures (diffuse, specular, normal ou height maps)
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numNormal = 0;
	unsigned int numHeight = 0;

	// Assigne les textures aux unités de textures en fonction de leur type (diffuse0, diffuse1... , specular0, specular1...)
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
			num = std::to_string(numDiffuse++);
		else if (type == "specular")
			num = std::to_string(numSpecular++);
		else if (type == "normal")
			num = std::to_string(numNormal++);
		else if (type == "height")
			num = std::to_string(numHeight++);
		
		// Assigne la texture à l'unité de texture correspondante, l'envoie au shader et la lie
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	std::cout << "Drawing mesh with " << indices.size() << " indices." << std::endl;

	// Dessine le Mesh actuel
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	VAO.Unbind();
}
