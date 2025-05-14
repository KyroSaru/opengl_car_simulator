#include "Model.h"
#include "Frustum.h"
#include "Car.h"
#include"JoystickManager.h"
#include"Gamepad.h"
#include "World.h"

#include <chrono>

const unsigned int width = 1600;
const unsigned int height = 800;

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

	// Active le V-Sync
	glfwSwapInterval(1);

	// Charge GLAD et donc les fonctions OpenGL (équivalent de GLEW)
	gladLoadGL();

	// Définit la zone de rendu (viewport) de la fenêtre (de x = 0, y = 0 à x = 800, y = 600)
	// Ici, on prend toute la fenêtre, mais on pourrait ne prendre qu'une partie
	glViewport(0, 0, width, height); // zone de rendu

	// ------------------------------------------

	// Créer un Shader Program avec les Vertex Shader et Fragment Shader spécifiés
	Shader carShader("shaders/car.vert", "shaders/car.frag");
	Shader wireframeShader("shaders/wireframe.vert", "shaders/wireframe.frag");
	Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
	Shader defaultShader("shaders/default.vert", "shaders/default.frag");
	Shader phareShader("shaders/headlight.vert", "shaders/headlight.frag");
	
	// Activer le mode de profondeur (Z-buffer/Depth Buffer)
	glEnable(GL_DEPTH_TEST);
	// Activer le blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activer le backface culling (faire attention à comment les modèles sont crées dans Blender)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	World world(width, height, window);
	world.loadModels();

	// Init la dernière frame avant le rendu
	float lastFrame = static_cast<float>(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		// Calcul du Delta Time (temps entre 2 frames)
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(calculateFPS()));
		glfwSetWindowTitle(window, windowTitle.c_str());

		world.updateScene(deltaTime);
		world.renderScene(carShader, phareShader, wireframeShader, terrainShader, defaultShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Supprime tous les objets créés
	carShader.Delete();
	wireframeShader.Delete();
	terrainShader.Delete();
	defaultShader.Delete();
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
