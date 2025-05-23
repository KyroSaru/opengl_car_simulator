#ifndef WORLD_EDITOR_UI_H
#define WORLD_EDITOR_UI_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "World.h"
#include "MaterialColors.h"

class WorldEditorUI
{
public:
	WorldEditorUI(GLFWwindow* window, World& world);

	void setVisible(bool v) { isUIVisible = v; }
	bool isVisible() const { return isUIVisible; }

	void Show();

	// ---------------

	bool isFlatNormalsEnabled() const { return flatNormalsEnabled; }

private:
	World& world;
	bool isUIVisible = false;

	// Flag pour savoir si on interpole les normales
	bool flatNormalsEnabled = true;
};

#endif // WORLD_EDITOR_UI_H
