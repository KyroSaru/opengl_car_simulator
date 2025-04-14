#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp//scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "BoundingBox.h"

class Model
{
public:
	// Tous les meshs & répertoire du modèle
	std::vector<Mesh> meshes;
	std::string directory;

	// Pour éviter de charger plusieurs fois la même texture
	std::vector<Texture> loadedTextures;

	// Pour définir une bounding box autour du modèle (pour le frustum culling)
	BoundingBox boundingBox;

	// Constructeur qui charge le modèle à partir d'un chemin
	Model(const char* path);
	// Parcours tous les meshs et les dessine
	void Draw(Shader& shader);

private:
	// Charge un modèle à partir d'un fichier spécifié par son chemin
	void loadModel(const std::string& path);
	// Traite un noeud de la scène (récursif)
	void processNode(aiNode* node, const aiScene* scene);
	// Traite un mesh de la scène et le convertit en un objet Mesh
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// Charge les textures du matériau du Mesh pour la fonction d'au dessus
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// Méthode pour calculer la bounding box
	void calculateBoundingBox();
};

#endif // MODEL_h