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

	// Active le V-Sync
	glfwSwapInterval(1);

	// Charge GLAD et donc les fonctions OpenGL (�quivalent de GLEW)
	gladLoadGL();

	// D�finit la zone de rendu (viewport) de la fen�tre (de x = 0, y = 0 � x = 800, y = 600)
	// Ici, on prend toute la fen�tre, mais on pourrait ne prendre qu'une partie
	// glViewport(0, 0, width, height); // zone de rendu
	// G�rer dans World.cpp car gestion multijoueur (split screen)

	// ------------------------------------------

	// Cr�er les Shader Program avec les Vertex Shader et Fragment Shader sp�cifi�s
	Shader defaultShader("shaders/default.vert", "shaders/default.frag");
	Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
	Shader carShader("shaders/default.vert", "shaders/car.frag");
	Shader headlightShader("shaders/default.vert", "shaders/headlight.frag");
	Shader wireframeShader("shaders/default.vert", "shaders/wireframe.frag");
	
	// Activer le mode de profondeur (Z-buffer/Depth Buffer)
	glEnable(GL_DEPTH_TEST);
	// Activer le blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activer le backface culling (faire attention � comment les mod�les sont cr�es dans Blender)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Instancie le monde et charge les mod�les 3D
	World world(width, height, window);
	world.loadModels();

	// Ne pas prendre en compte le DPI, acc�l. Windows...
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	// Init. la derni�re frame avant le rendu
	float lastFrame = static_cast<float>(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		// Calcul du Delta Time (temps entre 2 frames)
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Met � jour le titre avec les FPS affich�s
		std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(calculateFPS()));
		glfwSetWindowTitle(window, windowTitle.c_str());

		// Met � jour et fait le rendu de la sc�ne
		world.updateScene(deltaTime);
		world.renderScene(defaultShader, terrainShader, carShader, headlightShader, wireframeShader);

		// Double Buffering donc swap des tampons avant/arri�re et Gestion des inputs
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Supprime tous les objets cr��s
	defaultShader.Delete();
	terrainShader.Delete();
	carShader.Delete();
	headlightShader.Delete();
	wireframeShader.Delete();
	
	// Supprime la fen�tre avant la fin du programme
	glfwDestroyWindow(window);
	// Termine GLFW proprement
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
