#include "Terrain.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

Terrain::Terrain()
{
	// G�n�re le terrain � partir de la height map
	generateFromHeightMap(maxHeight, step);
}

void Terrain::generateFromHeightMap(float maxHeight, int step)
{
    // Charge la height map
    int channels;
    unsigned char* data = stbi_load(heightMapPath.c_str(), &width, &height, &channels, 1);
    if (!data)
    {
        std::cerr << "Failed to load height map: " << heightMapPath << std::endl;
        return;
    }

    // Calcule les d�calages pour centrer le terrain au milieu de l'univers (et non son coin)
    float offsetX = (width * scale) / 2.0f;
    float offsetZ = (height * scale) / 2.0f;

    // G�n�re les sommets
    for (int z = 0; z < height; z += step)
    {
        for (int x = 0; x < width; x += step)
        {
			// z : ligne, x : colonne
            float heightValue = data[z * width + x] / 255.0f * maxHeight;

            // Ajoute un bruit proc�dural (entre 0 et 0.7)
            float noise = (std::rand() % 100) / 100.0f * 0.7f;
            heightValue += noise;

            Vertex vertex;
            vertex.position = glm::vec3(x * scale - offsetX, heightValue, z * scale - offsetZ);
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f); // Normales par d�faut (on les recalcule plus bas)
            vertex.texCoords = glm::vec2(x / (float)width, z / (float)height);
            vertices.push_back(vertex);
        }
    }

    // G�n�re les indices (avec la possibilit� d'�chantilloner pour r�duire la densit� de sommets)
    int reducedWidth = (width + step - 1) / step;
    int reducedHeight = (height + step - 1) / step;

    for (int z = 0; z < reducedHeight - 1; ++z)
    {
        for (int x = 0; x < reducedWidth - 1; ++x)
        {
            int topLeft = z * reducedWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * reducedWidth + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

	// Lib�re la m�moire de la height map
    stbi_image_free(data);

    // Calcule les normales
    calculateNormals();

    // G�n�re les buffers avec VAO/VBO/EBO
    VAO.Bind();
    VBO vbo(vertices);
    EBO ebo(indices);

	// Lier le VBO et l'EBO au VAO
    VAO.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
    VAO.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Normales
    VAO.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // UV

	// D�lier le VAO, VBO et EBO
    VAO.Unbind();
    vbo.Unbind();
    ebo.Unbind();

	// MAJ du nombre de vertices (n�cessaire pour le Draw)
    vertexCount = static_cast<unsigned int>(indices.size());
}

void Terrain::regenerateTerrain(float maxHeight, int step)
{
	// Lib�re les anciennes infos du terrain
	vertices.clear();
	indices.clear();

	// Reg�n�re le terrain modifi�
	generateFromHeightMap(maxHeight, step);
}


void Terrain::Draw(Shader& shader)
{
    shader.Activate();

    shader.setMat4("model", terrainModelMatrix);

    VAO.Bind();
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    VAO.Unbind();
}

void Terrain::calculateNormals()
{
    // Parcourt tous les triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        // R�cup�re les indices des sommets du triangle
        GLuint index0 = indices[i];
        GLuint index1 = indices[i + 1];
        GLuint index2 = indices[i + 2];

        // R�cup�re les positions des sommets
        glm::vec3 v0 = vertices[index0].position;
        glm::vec3 v1 = vertices[index1].position;
        glm::vec3 v2 = vertices[index2].position;

        // Calcule la normale du triangle (produit vectoriel normalis�)
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
    // Inverse la matrice de transformation du terrain
    glm::mat4 inverseModel = glm::inverse(terrainModelMatrix);

    // Conversion des coordonn�es globales (worldX, worldZ) dans l'espace local du terrain
    glm::vec4 localPos = inverseModel * glm::vec4(worldX, 0.0f, worldZ, 1.0f);

    // Coordonn�es du terrain en prenant en compte l'�chelle
    float terrainX = localPos.x + (width * scale) / 2.0f;
    float terrainZ = localPos.z + (height * scale) / 2.0f;

    // Taille d'une cellule de la grille
    float gridSquareSize = scale * step;

    // Indices de la cellule dans laquelle se trouve le point
    int reducedWidth = (width + step - 1) / step;
    int gridX = static_cast<int>(std::floor(terrainX / gridSquareSize));
    int gridZ = static_cast<int>(std::floor(terrainZ / gridSquareSize));

    // V�rifier si le point est en dehors des limites du terrain
    if (gridX >= reducedWidth - 1 || gridZ >= reducedWidth - 1 || gridX < 0 || gridZ < 0) {
        return 0.0f;
    }

    // Coordonn�es locales dans la cellule
    float xCoord = (terrainX - gridX * gridSquareSize) / gridSquareSize;
    float zCoord = (terrainZ - gridZ * gridSquareSize) / gridSquareSize;

    // Calcul de la hauteur en fonction du triangle dans lequel se trouve le point
    float height;
    if (xCoord <= (1 - zCoord)) {
        // Triangle sup�rieur gauche
        height = barryCentric(
            glm::vec3(0, vertices[gridZ * reducedWidth + gridX].position.y, 0),
            glm::vec3(1, vertices[gridZ * reducedWidth + (gridX + 1)].position.y, 0),
            glm::vec3(0, vertices[(gridZ + 1) * reducedWidth + gridX].position.y, 1),
            glm::vec2(xCoord, zCoord)
        );
    }
    else {
        // Triangle inf�rieur droit
        height = barryCentric(
            glm::vec3(1, vertices[gridZ * reducedWidth + (gridX + 1)].position.y, 0),
            glm::vec3(1, vertices[(gridZ + 1) * reducedWidth + (gridX + 1)].position.y, 1),
            glm::vec3(0, vertices[(gridZ + 1) * reducedWidth + gridX].position.y, 1),
            glm::vec2(xCoord, zCoord)
        );
    }

    // Convertit la hauteur calcul�e dans l'espace global
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
