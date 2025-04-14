#include "Model.h"
#include "Frustum.h"

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
	
	// Activer le mode de profondeur (Z-buffer/Depth Buffer)
	glEnable(GL_DEPTH_TEST);

	// Activer le backface culling (faire attention � comment les mod�les sont cr�es dans Blender)
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	

	// Cr�er une cam�ra
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Charger les mod�les
	Model voiture("models/voiture_gltf/voiture.gltf");
	/*
	Model roueavg("models/voiture/voiture_roue1/voiture_roue_avg.obj");
	Model roueavd("models/voiture/voiture_roue2/voiture_roue_avd.obj");
	Model rouearg("models/voiture/voiture_roue3/voiture_roue_arg.obj");
	Model roueard("models/voiture/voiture_roue4/voiture_roue_ard.obj");
	*/
	// Vecteur de mod�les pour le frustum culling
	std::vector<Model*> models = { &voiture };

	// V�rifiez que le mod�le est charg�
	if (voiture.meshes.empty()) {
		std::cerr << "Erreur: Le mod�le n'a pas �t� charg� correctement." << std::endl;
		return -1;
	}

	// ------------------------------------------

	// Boucle principale de rendu
	while (!glfwWindowShouldClose(window))
	{
		// Mettre � jour le titre de la fen�tre (dans la boucle de rendu car on affiche les FPS)
		float fps = calculateFPS();
		std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(fps));
		glfwSetWindowTitle(window, windowTitle.c_str());

		// D�finit la couleur de fond de la fen�tre (RGBA)
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Nettoie le back buffer et le depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// G�re les entr�es de la cam�ra
		camera.Inputs(window);

		// Transformations des matrices vue/projection
		shaderProgram.Activate();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		// Calcule le frustum gr�ce au matrice projection et vue pour effectuer le culling bas� sur celui-ci
		Frustum frustum;
		frustum.calculateFrustum(projection, view);

		// Transformations du mod�le
		glm::mat4 mat_model = glm::mat4(1.0f);
		mat_model = glm::translate(mat_model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translation au centre de l'univers
		// model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Pour la mettre � l'endroit
		mat_model = glm::scale(mat_model, glm::vec3(1.0f, 1.0f, 1.0f));	// Homot�thie pour r�duire la taille du mod�le
		shaderProgram.setMat4("model", mat_model);

		// Dessiner le mod�le (en prenant en compte sa bounding box et en utilisant le frustum culling)
		std::vector<Model*> visibleModels = frustum.getVisibleModels(models);
		for (const auto& model : visibleModels) {
			std::cout << "BoundingBox Min: " << model->boundingBox.min.x << ", " << model->boundingBox.min.y << ", " << model->boundingBox.min.z << std::endl;
			std::cout << "BoundingBox Max: " << model->boundingBox.max.x << ", " << model->boundingBox.max.y << ", " << model->boundingBox.max.z << std::endl;
			model->Draw(shaderProgram);
		}

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
