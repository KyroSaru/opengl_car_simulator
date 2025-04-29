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