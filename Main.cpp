#include "Model.h"
#include "Frustum.h"
#include "Car.h"

#include <chrono>

#include"JoystickManager.h"
#include"Gamepad.h"

const unsigned int width = 800;
const unsigned int height = 600;

float calculateFPS();

int main()
{
	// Initialisation de GLFW
	glfwInit();

	// Dire la version d'OpenGL que l'on souhaite utiliser à GLFW (4.2)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// Dire à GLFW de ne pas utiliser les anciennes fonctions d'OpenGL (Core Profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Création de la fenêtre de <width>x<height> pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "Simulateur Automobile", NULL, NULL); // largeur, hauteur, titre, mode plein écran, fenêtre partagée
	// Vérification des erreurs si la fenêtre n'a pas pu être créée
	if (window == NULL)
	{
		std::cout << "La fenêtre GLFW n'a pas pu être créée" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Définit la fenêtre au contexte courant (celle qui sera utilisée)
	glfwMakeContextCurrent(window);

	// Charge GLAD et donc les fonctions OpenGL (équivalent de GLEW)
	gladLoadGL();

	// Définit la zone de rendu (viewport) de la fenêtre (de x = 0, y = 0 à x = 800, y = 600)
	// Ici, on prend toute la fenêtre, mais on pourrait ne prendre qu'une partie
	glViewport(0, 0, width, height); // zone de rendu

	// ------------------------------------------

	// Créer un Shader Program avec les Vertex Shader et Fragment Shader spécifiés
	Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
	Shader wireframeShader("shaders/wireframe.vert", "shaders/wireframe.frag");
	
	// Activer le mode de profondeur (Z-buffer/Depth Buffer)
	glEnable(GL_DEPTH_TEST);

	// Activer le backface culling (faire attention à comment les modèles sont crées dans Blender)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Activer le blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Créer une caméra
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Charger les modèles
	Car voiture("models/voiture_gltf/Car.gltf");
	//voiture.setGamepad(std::make_shared<Gamepad>(0));
	Model carte("models/sol_gltf/sol_gravier.gltf");

	// Vecteur de modèles pour le frustum culling
	std::vector<Model*> models = { &carte };

	// ------------------------------------------

	// Init la dernière frame avant le rendu
	float lastFrame = static_cast<float>(glfwGetTime());
	JoystickManager joystickManager;


	// Boucle principale de rendu
	while (!glfwWindowShouldClose(window))
	{

		// Mettre à jour le titre de la fenêtre (dans la boucle de rendu car on affiche les FPS)
		float fps = calculateFPS();
		std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(fps));
		glfwSetWindowTitle(window, windowTitle.c_str());

		// Calcul du Delta Time (temps entre 2 frames)
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Gestion dynamique de la manette 
		// note : ajouter des id/noms aux voitures
		if (!voiture.hasGamepad()) {
			auto gamepad = joystickManager.findGamepad();
			if (gamepad) {
				voiture.setGamepad(gamepad);
				std::cout << "Manette n°" << gamepad->jid() << "connectée ; Nom de la manette : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
			}
		}
		else if (voiture.hasGamepad() && !joystickManager.isConnected(voiture.getGamepad()->jid())) {
			voiture.setGamepad(nullptr);
			std::cout << "Manette déconnectée ! Reprise des contrôles clavier.\n";
		}
		// Gestion de la Physique en 3eme pers.
		if (camera.getMode() == THIRD_PERSON)
		{
			voiture.updatePhysics(deltaTime, window); // ou alors mettre les methodes du manager en static
			//std::cout << "Car pos: [x:" << voiture.getPosition().x << ", y:" << voiture.getPosition().y << ", z:" << voiture.getPosition().z << "]" << std::endl;
		}

		// Définit la couleur de fond de la fenêtre (RGBA)
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Nettoie le back buffer et le depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// ----------------------------------------
		// [CAMERA / FRUSTUM] 
		// Gére les entrées de la caméra
		camera.Inputs(window);
		// Met à jour la caméra en fonction de son mode et de la voiture
		camera.update(voiture.getPosition(), voiture.getDirection());

		// Transformations des matrices vue/projection
		shaderProgram.Activate();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		// Calcule le frustum grâce au matrice projection et vue pour effectuer le culling basé sur celui-ci
		Frustum frustum;
		frustum.calculateFrustum(projection, view);

		// ----------------------------------------

		// [MODELE DE VOITURE]
		// Dessiner la voiture et les autres modèles (en prenant en compte leur bounding box et en utilisant le frustum culling)
		if (voiture.isVisible(frustum))
		{
			voiture.Draw(shaderProgram);
			voiture.DrawWireframes(wireframeShader, view, projection);
		}

		// ----------------------------------------

		// Swap les buffers (de back à front / affiche le rendu)
		glfwSwapBuffers(window);
		// Événements/Inputs GLFW (redimens., fermeture, clavier, souris...)
		glfwPollEvents();
	}

	// Supprime tous les objets créés
	shaderProgram.Delete();
	// Supprime la fenêtre avant la fin du programme
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
