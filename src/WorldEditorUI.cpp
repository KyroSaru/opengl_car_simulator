#include "WorldEditorUI.h"

WorldEditorUI::WorldEditorUI(GLFWwindow* window, World& world)
    : world(world)
{
    // Initialise ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
}

void WorldEditorUI::Show() {
	// Vérifie si l'UI est visible
    if (!isUIVisible) return;

    ImGui::Begin("Simulation Settings");
    {
        // ------------ [TERRAIN] ------------
        // Flag pour savoir si une modification a été apporté au terrain
        bool terrainChanged = false;

        ImGui::Text("Terrain");
        // Hauteur du terrain
        float terrainHeight = world.getTerrainHeight();
        if (ImGui::SliderFloat("Terrain Height", &terrainHeight, 10.0f, 150.0f)) {
            world.setTerrainHeight(terrainHeight);
            terrainChanged = true;
        }
        // Pas d'échantillonnage
        int step = world.getTerrain().getStep();
        if (ImGui::SliderInt("Terrain Vertex Step (increase/reduce terrain vertices)", &step, 1, 10)) {
            world.getTerrain().setStep(step);
            terrainChanged = true;
        }
	    // Type de normales (effet lisse ou anguleux)
        if (ImGui::Checkbox("Flat Normals", &flatNormalsEnabled)) {
		    terrainChanged = true;
        }
        // Couleur du terrain
        ImGui::ColorEdit3("Ambient Terrain", glm::value_ptr(world.getTerrainColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Terrain", glm::value_ptr(world.getTerrainColors().diffuseColor));

	    // Regénère le terrain si changement
	    if (terrainChanged) {
		    // Regénère le terrain
		    world.getTerrain().regenerateTerrain(terrainHeight, step);
            // Regénère les modèles par rapport au terrain
		    world.regenerateModels(world.getCactiModelMatrices());
		    world.regenerateModels(world.getRocksModelMatrices());
		    world.regenerateModels(world.getWoodsModelMatrices());
		    world.regenerateModels(world.getGrassModelMatrices());
		    world.regenerateModels(world.getBonesModelMatrices());

            terrainChanged = false;
	    }

        ImGui::Separator();

        // ------------ [CYCLE JOUR-NUIT] ------------
        ImGui::Text("Day-Night Cycle");
        float duration = world.getDayNightCycleDuration();
        if (ImGui::SliderFloat("Day Night Cycle Duration (s)", &duration, 5.0f, 180.0f)) {
            world.setDayNightCycleDuration(duration);
        }

        float hour = world.getDayNightCycleHour();
        if (ImGui::SliderFloat("Cycle Hour (0 = midnight, 12 = noon)", &hour, 0.0f, 24.0f)) {
            world.setDayNightCycleHour(hour);
        }

        bool lockHour = world.getLockProgress();
        if (ImGui::Checkbox("Lock Cycle Hour", &lockHour)) {
            world.setLockProgress(lockHour);
        }

        ImGui::Separator();

        // ------------ [VOITURE] ------------
	    for (int i = 0; i < world.getCars().size(); ++i) {
            std::string label = "Car " + std::to_string(i + 1);
            ImGui::Text(label.c_str());

            // Ajout d'un identifiant unique pour  chaque voiture
            ImGui::PushID(i);
                ImGui::SliderFloat("Mouse Sensitivity", &world.getCars()[i].getCamera()->getSensitivity(), 1.0f, 200.0f);
                ImGui::SliderFloat("Gamepad Sensitivity", &world.getCars()[i].getCamera()->getGamepadSensitivity(), 1.0f, 300.0f);

                ImGui::SliderFloat("Max Speed", &world.getCars()[i].getMaxSpeed(), 5.0f, 30.0f);
                ImGui::SliderFloat("Max Steering Angle", &world.getCars()[i].getMaxSteeringAngle(), 5.0f, 90.0f);
                ImGui::SliderFloat("Steering Speed", &world.getCars()[i].getSteeringSpeed(), 10.0f, 200.0f);
                ImGui::SliderFloat("Body to Wheel Distance", &world.getCars()[i].getBodyToWheelDistance(), 0.1f, 0.6f);
                ImGui::SliderFloat("Bound Factor", &world.getCars()[i].getBounceFactor(), 0.0f, 1.0f);
                ImGui::ColorEdit3("Ambient Car", glm::value_ptr(world.getCars()[i].getCarsColors().ambientColor));
                ImGui::ColorEdit3("Diffuse Car", glm::value_ptr(world.getCars()[i].getCarsColors().diffuseColor));

                ImGui::Checkbox("Enable Wheel Trails", &world.getCars()[i].isWheelsTrailsEnabled());
                ImGui::ColorEdit3("Trail Color", glm::value_ptr(world.getCars()[i].getTrailColor()));
                ImGui::SliderFloat("Trail Thickness", &world.getCars()[i].getTrailThickness(), 0.1f, 2.0f);
                ImGui::SliderFloat("Trail Lifetime", &world.getCars()[i].getTrailDuration(), 0.1f, 5.0f);

                ImGui::Checkbox("Enable Weight Transfer (WIP)", &world.getCars()[i].isWeightTransferEnabled());
            ImGui::PopID();

            ImGui::Separator();
	    } 
    }
    ImGui::End();

    // -------------------------------------------

	ImGui::Begin("Randomly generated models");
    {
        // Flag pour savoir si le scale globale a été modifié ou si une nouvelle génération est nécessaire
	    bool globalScaleChanged = false;
	    bool newGenNeeded = false;

	    // Homothétie globale des modèles
        ImGui::Text("Global Models Changes");
	    float globalScale = world.getGlobalScale();
	    if (ImGui::SliderFloat("Global Scale", &globalScale, 0.0f, 5.0f)) {
		    world.setGlobalScale(globalScale);
            globalScaleChanged = true;
	    }

        ImGui::Separator();

        // Cactus
        ImGui::Text("Cacti");
        if (ImGui::SliderInt("Cacti Count", &world.getCactiParams().count, 0, 100) ||
            ImGui::SliderFloat("Cacti Min Scale", &world.getCactiParams().minScale, 0.0f, 5.0f) ||
            ImGui::SliderFloat("Cacti Max Scale", &world.getCactiParams().maxScale, 0.0f, 5.0f))
        { 
            auto& cacti = world.getCactiModelMatrices();
            cacti.clear();
            world.generateModels(cacti, world.getCactiParams().count, world.getCactiParams().minScale, world.getCactiParams().maxScale);
        }
        ImGui::ColorEdit3("Ambient Cacti", glm::value_ptr(world.getCactiColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Cacti", glm::value_ptr(world.getCactiColors().diffuseColor));

        ImGui::Separator();

        // Pierres
        ImGui::Text("Rocks");
        if (ImGui::SliderInt("Rocks Count", &world.getRocksParams().count, 0, 100) ||
            ImGui::SliderFloat("Rocks Min Scale", &world.getRocksParams().minScale, 0.0f, 5.0f) ||
            ImGui::SliderFloat("Rocks Max Scale", &world.getRocksParams().maxScale, 0.0f, 5.0f))
        {
            auto& rocks = world.getRocksModelMatrices();
            rocks.clear();
            world.generateModels(rocks, world.getRocksParams().count, world.getRocksParams().minScale, world.getRocksParams().maxScale);
        }
        ImGui::ColorEdit3("Ambient Rocks", glm::value_ptr(world.getRocksColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Rocks", glm::value_ptr(world.getRocksColors().diffuseColor));

        ImGui::Separator();

        // Bois
        ImGui::Text("Woods");
        if (ImGui::SliderInt("Woods Count", &world.getWoodsParams().count, 0, 100) ||
            ImGui::SliderFloat("Woods Min Scale", &world.getWoodsParams().minScale, 0.0f, 5.0f) ||
            ImGui::SliderFloat("Woods Max Scale", &world.getWoodsParams().maxScale, 0.0f, 5.0f))
        {
            auto& woods = world.getWoodsModelMatrices();
            woods.clear();
            world.generateModels(woods, world.getWoodsParams().count, world.getWoodsParams().minScale, world.getWoodsParams().maxScale);
        }
        ImGui::ColorEdit3("Ambient Woods", glm::value_ptr(world.getWoodsColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Woods", glm::value_ptr(world.getWoodsColors().diffuseColor));

        ImGui::Separator();

        // Herbe
        ImGui::Text("Grass");
		if (ImGui::SliderInt("Grass Count", &world.getGrassParams().count, 0, 100) ||
            ImGui::SliderFloat("Grass Min Scale", &world.getGrassParams().minScale, 0.0f, 5.0f) ||
            ImGui::SliderFloat("Grass Max Scale", &world.getGrassParams().maxScale, 0.0f, 5.0f))
        {
            auto& grass = world.getGrassModelMatrices();
            grass.clear();
            world.generateModels(grass, world.getGrassParams().count, world.getGrassParams().minScale, world.getGrassParams().maxScale);
        }
        ImGui::ColorEdit3("Ambient Grass", glm::value_ptr(world.getGrassColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Grass", glm::value_ptr(world.getGrassColors().diffuseColor));

        ImGui::Separator();

        // Os
        ImGui::Text("Bones");
		if (ImGui::SliderInt("Bones Count", &world.getBonesParams().count, 0, 100) ||
            ImGui::SliderFloat("Bones Min Scale", &world.getBonesParams().minScale, 0.0f, 5.0f) ||
            ImGui::SliderFloat("Bones Max Scale", &world.getBonesParams().maxScale, 0.0f, 5.0f))
        {
            auto& bones = world.getBonesModelMatrices();
            bones.clear();
            world.generateModels(bones, world.getBonesParams().count, world.getBonesParams().minScale, world.getBonesParams().maxScale);
        }
        ImGui::ColorEdit3("Ambient Bones", glm::value_ptr(world.getBonesColors().ambientColor));
        ImGui::ColorEdit3("Diffuse Bones", glm::value_ptr(world.getBonesColors().diffuseColor));


		// Génération des modèles
	    if (globalScaleChanged) {
		    world.regenerateModels(world.getCactiModelMatrices());
		    world.regenerateModels(world.getRocksModelMatrices());
		    world.regenerateModels(world.getWoodsModelMatrices());
		    world.regenerateModels(world.getGrassModelMatrices());
		    world.regenerateModels(world.getBonesModelMatrices());

            globalScaleChanged = false;
	    }
    }
    ImGui::End();
}
