#include "Model.h"
#include "Frustum.h"
#include "Car.h"
#include "World.h"

#include <chrono>

#include"JoystickManager.h"
#include"Gamepad.h"

const unsigned int width = 800;
const unsigned int height = 600;

float calculateFPS();

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Simulateur Automobile", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Erreur GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
    Shader wireframeShader("shaders/wireframe.vert", "shaders/wireframe.frag");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    World world(width, height, window);
    world.load();

    float lastFrame = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float fps = calculateFPS();
        std::string windowTitle = "Simulateur Automobile - FPS: " + std::to_string(static_cast<int>(fps));
        glfwSetWindowTitle(window, windowTitle.c_str());

        world.update(deltaTime);
        world.draw(shaderProgram, wireframeShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    wireframeShader.Delete();
    glfwDestroyWindow(window);
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
