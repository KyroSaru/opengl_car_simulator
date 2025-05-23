#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "Shader.h"

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;
	// Stocke le chemin de la texture pour ne pas la charger plusieurs fois (petite opti)
	std::string path;

	Texture(const char* image, const char* texType, GLuint slot);

	// Assigne une texture à une unité de texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Lie un texture
	void Bind();
	// Délie un texture
	void Unbind();
	// Supprime un texture
	void Delete();
};

#endif // TEXTURE_H