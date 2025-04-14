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
	// Tous les meshs & r�pertoire du mod�le
	std::vector<Mesh> meshes;
	std::string directory;

	// Pour �viter de charger plusieurs fois la m�me texture
	std::vector<Texture> loadedTextures;

	// Pour d�finir une bounding box autour du mod�le (pour le frustum culling)
	BoundingBox boundingBox;

	// Constructeur qui charge le mod�le � partir d'un chemin
	Model(const char* path);
	// Parcours tous les meshs et les dessine
	void Draw(Shader& shader);

private:
	// Charge un mod�le � partir d'un fichier sp�cifi� par son chemin
	void loadModel(const std::string& path);
	// Traite un noeud de la sc�ne (r�cursif)
	void processNode(aiNode* node, const aiScene* scene);
	// Traite un mesh de la sc�ne et le convertit en un objet Mesh
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// Charge les textures du mat�riau du Mesh pour la fonction d'au dessus
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// M�thode pour calculer la bounding box
	void calculateBoundingBox();
};

#endif // MODEL_h