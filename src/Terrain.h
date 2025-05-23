#ifndef TERRAIN_H
#define TERRAIN_H

#include "Shader.h"
#include "VAO.h"
#include "EBO.h"

#include <stb/stb_image.h>
#include <glm/glm.hpp>

class Terrain
{
public:
    // Constructeur de terrain
    Terrain();

    // Getters
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	float getScale() const { return scale; }
    int getStep() const { return step; }
    void setStep(int s) { step = s; }

    // Définit la matrice modèle du terrain
    void setModelMatrix(const glm::mat4& modelMatrix) { terrainModelMatrix = modelMatrix; }
    // Obtient la hauteur à une position donnée sur le plan (x, z) pour les roues de Car::updatePhysics
    float getHeightAt(float worldX, float worldZ) const;

    // Regénère le terrain dynamiquement pour l'UI
    void regenerateTerrain(float maxHeight, int step);

    // Dessine le terrain
    void Draw(Shader& shader);

private:
	// Chemin de la height map
    const std::string& heightMapPath = "assets/textures/terrain/heightmap.png";

	// VAO (VBO/EBO) pour gérer les buffers OpenGL et nombre de vertices
    VAO VAO;
    unsigned int vertexCount;

    // Dimensions et échelle de la height map, hauteur max du terrain & échantillonnage pour densité des sommets
	int width, height; 
	float scale = 2.0f;
    float maxHeight = 45.0f;
    int step = 3;

    // Sommets et indices du terrain
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    // Matrice de Model du terrain
    glm::mat4 terrainModelMatrix = glm::mat4(1.0f);

    // -------------------------------

	// Génère le terrain en précisant le chemin d'une height map, une hauteur max. et un pas
    void generateFromHeightMap(float maxHeight, int step);

    // Calcule les normales du terrain
    void calculateNormals();

    // Utilise les coord. barycentriques pour interpoler et retourner la hauteur y à une position donnée dans un triangle
    float barryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos) const;
};

#endif // TERRAIN_H