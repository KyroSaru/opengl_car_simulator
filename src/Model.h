#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp//scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "BoundingBox.h"
#include "Frustum.h"

class Model
{
public:
	// Tous les meshs & répertoire du modèle
	std::vector<Mesh> meshes;
	std::string directory;

	// Pour éviter de charger plusieurs fois la même texture
	std::vector<Texture> loadedTextures;

	// Bounding boxs autour du modèle (pour le frustum culling/visibilité + pour les collisions)
	BoundingBox frustumBox;
	BoundingBox collisionBox;

	// Constructeur qui charge le modèle à partir d'un chemin (ou un noeud/partie d'un modèle)
	Model() = default; // pour déclarer sans init dans Car.h
	Model(const std::string& path);
	Model(const std::string& path, const std::string& nodeName);

	// Getter des meshes
	const std::vector<Mesh>& getMeshes() const { return meshes; }

	// Parcours tous les meshs et les dessine
	void Draw(Shader& shader);

	// Méthode pour savoir si un modèle est visible dans le frustum
	bool isVisible(const Frustum& frustum, const glm::mat4& modelMatrix) const;
private:
	// Charge un modèle à partir d'un fichier spécifié par son chemin
	void loadModel(const std::string& path);
	// Cherche un noeud par son nom & charge le noeud spécifié
	aiNode* findNodeByName(aiNode* node, const std::string& name);
	void loadSpecificNode(const std::string& path, const std::string& nodeName);

	// Traite un noeud de la scène (récursif)
	void processNode(aiNode* node, const aiScene* scene);
	// Traite un mesh de la scène et le convertit en un objet Mesh
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// Charge les textures du matériau du Mesh pour la fonction d'au dessus
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// Méthode pour calculer les 2 boxes
	void calculateBoundingBoxes();
};

#endif // MODEL_h