#include "Model.h"
#include "Frustum.h"
#include "Car.h"
#include "JoystickManager.h"

#include <chrono>

const unsigned int width = 800;
const unsigned int height = 600;

float calculateFPS();

int main()
{
	// Initialisation de GLFW
	glfwInit();

	// Dire la version d'OpenGL que l'on souhaite utiliser � GLFW (4.2)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// Dire � GLFW de ne pas utiliser les anciennes fonctions d'OpenGL (Core Profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Cr�ation de la fen�tre de <width>x<height> pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "Simulateur Automobile", NULL, NULL); // largeur, hauteur, titre, mode plein �cran, fen�tre partag�e
	// V�rification des erreurs si la fen�tre n'a pas pu �tre cr��e
	if (window == NULL)
	{
		std::cout << "La fen�tre GLFW n'a pas pu �tre cr��e" << std::endl;
		glfwTerminate();
		return -1;
	}
	// D�finit la fen�tre au contexte courant (celle qui sera utilis�e)
	glfwMakeContextCurrent(window);

	// Charge GLAD et donc les fonctions OpenGL (�quivalent de GLEW)
	gladLoadGL();

	// D�finit la zone de rendu (viewport) de la fen�tre (de x = 0, y = 0 � x = 800, y = 600)
	// Ici, on prend toute la fen�tre, mais on pourrait ne prendre qu'une partie
	glViewport(0, 0, width, height); // zone de rendu

	// ------------------------------------------

	// Cr�er un Shader Program avec les Vertex Shader et Fragment Shader sp�cifi�s
	Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
	Shader wireframeShader("shaders/wireframe.vert", "shaders/wireframe.frag");
	Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
	
	// Activer le mode de profondeur (Z-buffer/Depth Buffer)
	glEnable(GL_DEPTH_TEST);

	// Activer le backface culling (faire attention � comment les mod�les sont cr�es dans Blender)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Cr�er une cam�ra
	auto camera = std::make_shared<Camera>(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Charger les mod�les
	Car voiture("models/voiture_gltf/voiture.gltf");
	Model carte("models/map/scene.gltf");

	Terrain terrain("models/terrain/heightmap.png", 25.0f);

	// Vecteur de mod�les pour le frustum culling
	std::vector<Model*> culledModels = { &carte };

	// [GESTION DES CONTROLLEURS]
	std::shared_ptr<Keyboard> keyboard = std::make_shared<Keyboard>();
	// voiture.setKeyboard(keyboard);

	auto availableGamepads = JoystickManager::findAllAvailableGamepads();
	auto gamepad = availableGamepads.back();
	voiture.setGamepad(gamepad);
	voiture.setCamera(camera);
	voiture.getCamera()->setGamepad(gamepad);

	// ------------------------------------------

	// Init la derni�re frame avant le rendu
	float lastFrame = static_cast<float>(glfwGetTime());

	// Boucle principale de rendu
	while (!glfwWindowShouldClose(window))
	{
		// Mettre � jour le titre de la fen�tre (dans la boucle de rendu car on affiche les FPS)
		float fps = calculateFPS();
		std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(fps));
		glfwSetWindowTitle(window, windowTitle.c_str());

		// Calcul du Delta Time (temps entre 2 frames)
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Gestion de la Physique en 3eme pers.
		if (voiture.getCamera()->getMode() == THIRD_PERSON)
		{
			voiture.updatePhysics(deltaTime, window, terrain);
			std::cout << "Car pos: [x:" << voiture.getPosition().x << ", y:" << voiture.getPosition().y << ", z:" << voiture.getPosition().z << "]" << std::endl;
		}

		// D�finit la couleur de fond de la fen�tre (RGBA)
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Nettoie le back buffer et le depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ----------------------------------------

		// [CAMERA / FRUSTUM] 
		// G�re les entr�es de la cam�ra
		voiture.getCamera()->Inputs(window);
		// Met � jour la cam�ra en fonction de son mode et de la voiture
		voiture.getCamera()->update(voiture.getBodyPosition(), voiture.getDirection(), terrain);

		// Transformations des matrices vue/projection
		shaderProgram.Activate();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 200.0f);
		glm::mat4 view = voiture.getCamera()->getViewMatrix();
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		// Calcule le frustum gr�ce au matrice projection et vue pour effectuer le culling bas� sur celui-ci
		Frustum frustum;
		frustum.calculateFrustum(projection, view);

		// ----------------------------------------

		// [MODELE DE VOITURE]
		// Dessiner la voiture et les autres mod�les (en prenant en compte leur bounding box et en utilisant le frustum culling)
		if (voiture.isVisible(frustum))
		{
			voiture.Draw(shaderProgram);
			// voiture.DrawWireframes(wireframeShader, view, projection);
		}

		/*
		// [MODELE DE DEBUG]
		// Position fixe pour que le mod�le de r�f�rence ne se d�place pas avec la voiture
		shaderProgram.Activate();
		glm::mat4 fixModelMatrix = glm::mat4(1.0f);
		fixModelMatrix = glm::translate(fixModelMatrix, glm::vec3(0.0f, -1.0f, -15.0f));
		fixModelMatrix = glm::rotate(fixModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		for (const auto& model : culledModels) {
			// Nouvelle boundingbox du mod�le apr�s transformation par model matrix
			BoundingBox transformedBox = model->boundingBox.getTransformed(fixModelMatrix);

			if (frustum.isInFrustum(transformedBox))
			{
				shaderProgram.setMat4("model", fixModelMatrix);
				std::cout << "BoundingBox Min: " << model->boundingBox.min.x << ", " << model->boundingBox.min.y << ", " << model->boundingBox.min.z << std::endl;
				std::cout << "BoundingBox Max: " << model->boundingBox.max.x << ", " << model->boundingBox.max.y << ", " << model->boundingBox.max.z << std::endl;
				model->Draw(shaderProgram);
			}
		}
		*/

		// Gestion du terrain ici pour le moment
		terrainShader.Activate();
		terrainShader.setMat4("view", view);
		terrainShader.setMat4("projection", projection);
		glm::mat4 terrainModel = glm::mat4(1.0f);
		terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -10.0f, 0.0f));
		terrain.setModelMatrix(terrainModel);
		
		terrainShader.setVec3("viewPos", voiture.getCamera()->getPosition()); // Position de la cam�ra pour shader
		terrain.Draw(terrainShader);

		// ----------------------------------------

		// Swap les buffers (de back � front / affiche le rendu)
		glfwSwapBuffers(window);
		// �v�nements/Inputs GLFW (redimens., fermeture, clavier, souris...)
		glfwPollEvents();
	}

	// Supprime tous les objets cr��s
	shaderProgram.Delete();
	// Supprime la fen�tre avant la fin du programme
	glfwDestroyWindow(window);
	// Termine GLFW
	glfwTerminate();
	return 0;
}

// ---------------------------------------------------

// Calcule les FPS
float calculateFPS() {
	static auto lastTime = std::chrono::high_resolution_clock::now();
	static int frameCount = 0;
	static float fps = 0.0f;

	auto currentTime = std::chrono::high_resolution_clock::now();
	frameCount++;

	float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();

	// MAJ une fois la seconde atteinte
	if (elapsedTime >= 1.0f) {
		fps = frameCount / elapsedTime;
		frameCount = 0;
		lastTime = currentTime;
	}

	return fps;
}
