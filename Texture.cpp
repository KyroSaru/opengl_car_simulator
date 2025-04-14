#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	// Assigne le type de texture à l'objet texture, son unité de texture et son chemin
	type = texType;
	unit = slot;
	path = std::string(image); // Initialise le chemin de la texture

	// Largeur, hauteur et nombre de canaux de couleur de l'image
	int widthImg, heightImg, numColCh;
	// Inverse l'image (car stb_image charge du coin supérieur gauche au coin inférieur droit alors qu'OpenGL charge du coin inférieur gauche au coin supérieur droit)
	stbi_set_flip_vertically_on_load(true);
	// Importe la texture avec stb_image (lit l'image et la convertit en octets)
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Génére une texture OpenGL
	glGenTextures(1, &ID);
	// Assigne la texture à l'unité spécifiée
	glActiveTexture(GL_TEXTURE0 + slot);
	// Lie la texture OpenGL à TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configure le type d'algo. utilisée pour minimiser et maximiser la texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Configurer de quelle manière la texture doit être répétée si nécessaire (s, t, r équivalent à x, y, z)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Assigne l'image à la texture OpenGL en fonction de son nombre de canaux de couleur
	if (numColCh == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RG, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	else {
		std::cerr << "[TEXTURE] " << path << "(" << numColCh << " channels)" << std::endl;
		throw std::invalid_argument("Texture format not supported by the texture loader (only 1, 2, 3 or 4 channels supported):");
	}
	
	// Génère les mipmaps (textures de différentes résolutions pour de meilleurs perf.)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Libère la mémoire de l'image importée (déjà dans l'objet OpenGL)
	stbi_image_free(bytes);
	// Délie la texture pour éviter les conflits
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) 
{
	// Obtenir la "location" du uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Le shader doit être activé avant d'utiliser glUniform
	shader.Activate();
	// Définir la valeur du uniform 
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}