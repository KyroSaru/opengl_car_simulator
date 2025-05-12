#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <glm/glm.hpp>

std::string getFileContents(const char* filename);

class Shader
{
public:
	// ID du Shader Program
	GLuint ID;
	// Constructeur qui crée le Shader Program de 2 shaders (Vertex/Fragment)
	Shader(const char* vertexFile, const char* fragmentFile);

	// Active le Shader Program
	void Activate();
	// Supprime le Shader Program
	void Delete();

	// Getter du nom du shader


	// ------------------------------------------------------------------------

	// Envoie un int au shader
	void setInt(const std::string& uniform, int value) const;
	// Envoie un float au shader
	void setFloat(const std::string& uniform, float value) const;
	// Envoie un vecteur 3D au shader
	void setVec3(const std::string& uniform, const glm::vec3& vec) const;
	// Envoie une matrice 4x4 au shader
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	// ------------------------------------------------------------------------
private:
	// Vérifie les erreurs de compilation et de lien des shaders
	void compileErrors(unsigned int shader, const char* type);
};
#endif // !SHADER_H
