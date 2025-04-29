#include "Terrain.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>

// Constructeur
Terrain::Terrain(const std::string& heightMapPath, float maxHeight)
{
    generateFromHeightMap(heightMapPath, maxHeight);
}

void Terrain::generateFromHeightMap(const std::string& heightMapPath, float maxHeight)
{
    // Charge la height map
    int channels;
    unsigned char* data = stbi_load(heightMapPath.c_str(), &width, &height, &channels, 1);
    if (!data)
    {
        std::cerr << "Failed to load height map: " << heightMapPath << std::endl;
        return;
    }

    // Calcule les décalages pour centrer le terrain au milieu de l'univers (et non son coin)
    float offsetX = (width * scale) / 2.0f;
    float offsetZ = (height * scale) / 2.0f;

    // Génére les sommets
    for (int z = 0; z < height; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
			// z : ligne, x : colonne
            float heightValue = data[z * width + x] / 255.0f * maxHeight;
            Vertex vertex;
            vertex.position = glm::vec3(x * scale - offsetX, heightValue, z * scale - offsetZ);
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f); // Normales par défaut (on les recalcule par la suite)
            vertex.texCoords = glm::vec2(x / (float)width, z / (float)height);
            vertices.push_back(vertex);
        }
    }

    // Génére les indices
    for (int z = 0; z < height - 1; ++z)
    {
        for (int x = 0; x < width - 1; ++x)
        {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

	// Libére la mémoire de la height map
    stbi_image_free(data);

    // Calcule les normales
    calculateNormals();

    // Génère les buffers avec VAO/VBO/EBO
    VAO.Bind();
    VBO vbo(vertices);
    EBO ebo(indices);

	// Lier le VBO et l'EBO au VAO
    VAO.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
    VAO.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Normales
    VAO.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // UV

	// Délier le VAO, VBO et EBO
    VAO.Unbind();
    vbo.Unbind();
    ebo.Unbind();

	// MAJ du nombre de vertices (nécessaire pour le Draw)
    vertexCount = indices.size();
}


void Terrain::Draw(Shader& shader)
{
    shader.Activate();

    // Envoie les uniforms de la lumière (direction, couleur blanche)
    shader.setVec3("lightDir", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)));
    shader.setVec3("lightColor", glm::vec3(1.0f, 0.9f, 0.9f));

    // Envoie les uniforms du matériau (composante ambiante, diffuse, spéculaire et brillance)
    shader.setVec3("ambientColor", glm::vec3(0.8f, 0.5f, 0.3f));
    shader.setVec3("diffuseColor", glm::vec3(0.7f, 0.5f, 0.4f));
    shader.setVec3("specularColor", glm::vec3(0.9f, 0.9f, 0.9f));
    shader.setFloat("shininess", 32.0f);

    shader.setMat4("model", terrainModelMatrix);

    VAO.Bind();
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    VAO.Unbind();
}

void Terrain::calculateNormals()
{
    // Parcourt tous les triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Récupére les indices des sommets du triangle
        GLuint index0 = indices[i];
        GLuint index1 = indices[i + 1];
        GLuint index2 = indices[i + 2];

        // Récupére les positions des sommets
        glm::vec3 v0 = vertices[index0].position;
        glm::vec3 v1 = vertices[index1].position;
        glm::vec3 v2 = vertices[index2].position;

		// Calcule la normale du triangle (produit vectoriel normalisé)
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 triangleNormal = glm::normalize(glm::cross(edge1, edge2));

        // Ajoute la normale du triangle aux normales des sommets
        vertices[index0].normal += triangleNormal;
        vertices[index1].normal += triangleNormal;
        vertices[index2].normal += triangleNormal;
    }
}



float Terrain::getHeightAt(float worldX, float worldZ) const {
    // Inverse la matrice de transfo du terrain
    glm::mat4 inverseModel = glm::inverse(terrainModelMatrix);

    // Conversion coord. globales (worldX, worldZ) dans l'espace local du terrain
    glm::vec4 localPos = inverseModel * glm::vec4(worldX, 0.0f, worldZ, 1.0f);

	// Coord. du terrain en prenant en compte l'échelle
    float terrainX = localPos.x + (width * scale) / 2.0f;
    float terrainZ = localPos.z + (height * scale) / 2.0f;

    // Taille d'une cellule de la grille
    float gridSquareSize = scale;

    // Indices de la cellule dans laquelle se trouve le point
    int gridX = static_cast<int>(std::floor(terrainX / gridSquareSize));
    int gridZ = static_cast<int>(std::floor(terrainZ / gridSquareSize));

    // Vérifier si le point est en dehors des limites du terrain
    if (gridX >= width - 1 || gridZ >= height - 1 || gridX < 0 || gridZ < 0) {
        return 0.0f;
    }

    // Coord. locales dans la cellule
    float xCoord = (terrainX - gridX * gridSquareSize) / gridSquareSize;
    float zCoord = (terrainZ - gridZ * gridSquareSize) / gridSquareSize;

    // Calcul de la hauteur en fonction du triangle dans lequel se trouve le point
    float height;
    if (xCoord <= (1 - zCoord)) {
        // Triangle supérieur gauche
        height = barryCentric(
            glm::vec3(0, vertices[gridZ * width + gridX].position.y, 0),
            glm::vec3(1, vertices[gridZ * width + (gridX + 1)].position.y, 0),
            glm::vec3(0, vertices[(gridZ + 1) * width + gridX].position.y, 1),
            glm::vec2(xCoord, zCoord)
        );
    }
    else {
        // Triangle inférieur droit
        height = barryCentric(
            glm::vec3(1, vertices[gridZ * width + (gridX + 1)].position.y, 0),
            glm::vec3(1, vertices[(gridZ + 1) * width + (gridX + 1)].position.y, 1),
            glm::vec3(0, vertices[(gridZ + 1) * width + gridX].position.y, 1),
            glm::vec2(xCoord, zCoord)
        );
    }

    // Convertit la hauteur calculée dans l'espace global
    glm::vec4 globalHeight = terrainModelMatrix * glm::vec4(0.0f, height, 0.0f, 1.0f);
    return globalHeight.y;
}



float Terrain::barryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos) const {
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;

    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
