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

// ordre cw pour afficher les faces intérieurs du cube
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
	: faces {
	"textures/skybox/blueSky/left.png",
	"textures/skybox/blueSky/right.png",
	"textures/skybox/blueSky/up.png",
	"textures/skybox/blueSky/down.png",
	"textures/skybox/blueSky/front.png",
	"textures/skybox/blueSky/back.png",
	}, 
	shader("shaders/skybox.vert", "shaders/skybox.frag")
{ 
	// VAO, VBO, EBO
	glGenVertexArrays(1, &skyboxVao);
	glGenBuffers(1, &skyboxVbo);
	glGenBuffers(1, &skyboxEbo);

	glBindVertexArray(skyboxVao);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapsVertices), &cubemapsVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubemapsIndices), &cubemapsIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// TEXTURE
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // x
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // y 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // z

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
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
			std::cout << "Failed to load texture: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
}

void Skybox::draw(const glm::mat4& view, const glm::mat4 projection)
{
	glDepthFunc(GL_LEQUAL); // derrière 

	shader.Activate();

	shader.setInt("skybox", 0);
	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
	shader.setMat4("view", viewNoTranslation);
	shader.setMat4("projection", projection);

	glBindVertexArray(skyboxVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}
