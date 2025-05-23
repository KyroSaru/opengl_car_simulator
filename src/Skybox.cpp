#include"Skybox.h"

float cubemapsVertices[] = {
	-1.0f, -1.0f,  1.0f, // Coin avant-bas gauche  (0)
	 1.0f, -1.0f,  1.0f, // Coin avant-bas droite  (1)
	 1.0f, -1.0f, -1.0f, // Coin arrière-bas droite (2)
	-1.0f, -1.0f, -1.0f, // Coin arrière-bas gauche (3)
	-1.0f,  1.0f,  1.0f, // Coin avant-haut gauche (4)
	 1.0f,  1.0f,  1.0f, // Coin avant-haut droite (5)
	 1.0f,  1.0f, -1.0f, // Coin arrière-haut droite (6)
	-1.0f,  1.0f, -1.0f  // Coin arrière-haut gauche (7)
};

// En clockwise pour afficher l'intérieur de la skybox
unsigned int cubemapsIndices[] =
{
	// face droite, +X
	6, 2, 1, // triangle 1
	1, 5, 6, // triangle 2
	// face gauche, -X
	7, 4, 0,
	0, 3, 7,
	// face dessus, +Y
	6, 5, 4,
	4, 7, 6,
	// face dessous, -Y
	2, 3, 0,
	0, 1, 2,
	// face arrière, -Z
	5, 1, 0,
	0, 4, 5,
	// face avant, +Z
	6, 7, 3,
	3, 2, 6
};

Skybox::Skybox()
	: dayFaces {
	"assets/textures/skybox/jour/left.png",
	"assets/textures/skybox/jour/right.png",
	"assets/textures/skybox/jour/up.png",
	"assets/textures/skybox/jour/down.png",
	"assets/textures/skybox/jour/front.png",
	"assets/textures/skybox/jour/back.png",
	},
	nightFaces {
	"assets/textures/skybox/nuit/left.png",
	"assets/textures/skybox/nuit/right.png",
	"assets/textures/skybox/nuit/up.png",
	"assets/textures/skybox/nuit/down.png",
	"assets/textures/skybox/nuit/front.png",
	"assets/textures/skybox/nuit/back.png",
	}, 
	skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag")
{ 
	// Génération des tampons et du VAO
	glGenVertexArrays(1, &skyboxVao);
	glGenBuffers(1, &skyboxVbo);
	glGenBuffers(1, &skyboxEbo);
	
	// Lie le VAO et les tampons au contexte OpenGL
	glBindVertexArray(skyboxVao);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapsVertices), &cubemapsVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubemapsIndices), &cubemapsIndices, GL_STATIC_DRAW);

	// Lie les attributs des tampons au VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Gestion des textures avec stb_image
	glGenTextures(1, &dayTextureID);
	glGenTextures(1, &nightTextureID);

	// Charge les 2 cubemaps
	loadCubeMap(dayTextureID, dayFaces);
	loadCubeMap(nightTextureID, nightFaces);

	// Lisse les bords de la skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Skybox::Draw(const glm::mat4& view, const glm::mat4 projection, float time)
{
	// Tout ce qu'il y a entre ces brackets est toujours derrière les autres objets (la skybox)
	glDepthFunc(GL_LEQUAL);
	{
		skyboxShader.Activate();
		
		// Active et lie la texture JOUR (unité 0)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dayTextureID);
		// Active et lie la texture NUIT (unité 1)
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, nightTextureID);

		skyboxShader.setInt("skyTextureDay", 0);
		skyboxShader.setInt("skyTextureNight", 1);

		// Transition entre le jour et la nuit
		skyboxShader.setFloat("blendValue", blendValue);

		// Supprime la translation de la matrice de vue (le ciel ne doit pas bouger avec la caméra)
		glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

		// Ajoute une rotation pour simuler le mouvement du ciel
		float rotationSpeed = 0.01f;
		glm::mat4 rotation = glm::rotate(viewNoTranslation, time * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

		skyboxShader.setMat4("view", rotation);
		skyboxShader.setMat4("projection", projection);

		// Lie le VAO et dessine la skybox
		glBindVertexArray(skyboxVao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glDepthFunc(GL_LESS);
}

// --------------------------

void Skybox::loadCubeMap(GLuint textureID, const std::string faces[6])
{
	// Lie la texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Gestion de l'interpolation des pixels de la texture (ici, pas des carrés mais lissés)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Gestion des coord. de textures si on dépasse les bords de la texture (ici, couleur de la texture sur le bord)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // <=> x
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // <=> y 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // <=> z

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "[SKYBOX] Failed to load texture: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
}
