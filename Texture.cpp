#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	// Assigne le type de texture � l'objet texture, son unit� de texture et son chemin
	type = texType;
	unit = slot;
	path = std::string(image); // Initialise le chemin de la texture

	// Largeur, hauteur et nombre de canaux de couleur de l'image
	int widthImg, heightImg, numColCh;
	// Inverse l'image (car stb_image charge du coin sup�rieur gauche au coin inf�rieur droit alors qu'OpenGL charge du coin inf�rieur gauche au coin sup�rieur droit)
	stbi_set_flip_vertically_on_load(true);
	// Importe la texture avec stb_image (lit l'image et la convertit en octets)
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// G�n�re une texture OpenGL
	glGenTextures(1, &ID);
	// Assigne la texture � l'unit� sp�cifi�e
	glActiveTexture(GL_TEXTURE0 + slot);
	// Lie la texture OpenGL � TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configure le type d'algo. utilis�e pour minimiser et maximiser la texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Configurer de quelle mani�re la texture doit �tre r�p�t�e si n�cessaire (s, t, r �quivalent � x, y, z)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Assigne l'image � la texture OpenGL en fonction de son nombre de canaux de couleur
	if (numColCh == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RG, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	else {
		std::cerr << "[TEXTURE] " << path << "(" << numColCh << " channels)" << std::endl;
		throw std::invalid_argument("Texture format not supported by the texture loader (only 1, 2, 3 or 4 channels supported):");
	}
	
	// G�n�re les mipmaps (textures de diff�rentes r�solutions pour de meilleurs perf.)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Lib�re la m�moire de l'image import�e (d�j� dans l'objet OpenGL)
	stbi_image_free(bytes);
	// D�lie la texture pour �viter les conflits
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) 
{
	// Obtenir la "location" du uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Le shader doit �tre activ� avant d'utiliser glUniform
	shader.Activate();
	// D�finir la valeur du uniform 
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