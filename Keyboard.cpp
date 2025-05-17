#include"Keyboard.h"

float Keyboard::getSteerInput(GLFWwindow* window) const
{
    float steer = 0.0f;

    if (isLeftPressed(window))
        steer += 1.0f;
    if (isRightPressed(window))
        steer -= 1.0f;

    return steer;
}

float Keyboard::getAccelInput(GLFWwindow* window) const
{
    float accel = 0.0f;

    if (isForwardPressed(window))
        accel += 1.0f;
    if (isBackwardPressed(window))
        accel -= 1.0f;

    return accel;
}

// ------------------

bool Keyboard::wasEnterPressed(GLFWwindow* window) const {
    static bool wasPressedLastFrame = false;

    // Retourne vrai si la touche Entr�e a �t� press�e
    bool isCurrentlyPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
    // Retourne vrai la premi�re fois que la touche Entr�e est press�e
    bool justReleased = wasPressedLastFrame && !isCurrentlyPressed;

    // MAJ le bool�en pour le prochain appel
    wasPressedLastFrame = isCurrentlyPressed;

    return justReleased;
}

bool Keyboard::wasBackspacePressed(GLFWwindow* window) const {
    static bool wasPressedLastFrame = false;

    bool isCurrentlyPressed = glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS;
    bool justReleased = wasPressedLastFrame && !isCurrentlyPressed;

    wasPressedLastFrame = isCurrentlyPressed;

    return justReleased;
}

bool Keyboard::wasHPressed(GLFWwindow* window) const {
    static bool wasPressedLastFrame = false;

    bool isCurrentlyPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;
    bool justReleased = wasPressedLastFrame && !isCurrentlyPressed;

    wasPressedLastFrame = isCurrentlyPressed;

    return justReleased;
}

bool Keyboard::wasWPressed(GLFWwindow* window) const {
    static bool wasPressedLastFrame = false;

    bool isCurrentlyPressed = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
    bool justReleased = wasPressedLastFrame && !isCurrentlyPressed;

    wasPressedLastFrame = isCurrentlyPressed;

    return justReleased;
}