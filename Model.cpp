#include "Model.h"

Model::Model(const std::string& path)
{
	loadModel(path);
	// Calcule la bounding box après avoir chargé le modèle
	calculateBoundingBox(); 
}

Model::Model(const std::string& path, const std::string& nodeName)
{
	loadSpecificNode(path, nodeName);
	calculateBoundingBox();
}


void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}


void Model::loadModel(const std::string& path)
{
	// Lit le fichier avec Assimp (avec options de traitement)
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	// Vérifie les erreurs de chargement
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[ASSIMP] Error: " << importer.GetErrorString() << std::endl; 
		return;
	}

	// Répertoire dans lequel se trouve le modèle
	directory = path.substr(0, path.find_last_of('/'));

	// Traite le noeud racine de la scène (et récursiv. ses enfants)
	processNode(scene->mRootNode, scene);
}

aiNode* Model::findNodeByName(aiNode* node, const std::string& name)
{
	if (node->mName.C_Str() == name)
		return node;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		aiNode* found = findNodeByName(node->mChildren[i], name);
		if (found)
			return found;
	}
	return nullptr;
}

void Model::loadSpecificNode(const std::string& path, const std::string& nodeName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[ASSIMP] Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	// Cherche le noeud voulu
	aiNode* targetNode = findNodeByName(scene->mRootNode, nodeName);
	if (targetNode)
		processNode(targetNode, scene);
	else
		std::cerr << "[Model] Node named '" << nodeName << "' not found in the scene." << std::endl;
}



void Model::processNode(aiNode* node, const aiScene* scene)
{
	std::cout << "Processing node: " << node->mName.C_Str() << std::endl;
	
	// Récupére tous les meshs du noeud, les traite et les ajoute au vecteur de meshs
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// L'objet node contient des indices vers les meshs de la scène (scène contient toutes les données)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// Traite récursiv. les enfants du noeud
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::cout << "Processing mesh: " << mesh->mName.C_Str() << std::endl;

	// Vecteurs pour stocker les vertices, indices et textures de notre Mesh
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Traite tous les vertices du mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 attrVec3;

		// Position
		attrVec3.x = mesh->mVertices[i].x;
		attrVec3.y = mesh->mVertices[i].y;
		attrVec3.z = mesh->mVertices[i].z;
		vertex.position = attrVec3;

		// Normals
		if (mesh->HasNormals())
		{
			attrVec3.x = mesh->mNormals[i].x;
			attrVec3.y = mesh->mNormals[i].y;
			attrVec3.z = mesh->mNormals[i].z;
			vertex.normal = attrVec3;
		}

		// Texture UVs (si présente)
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 attrVec2;
			attrVec2.x = mesh->mTextureCoords[0][i].x;
			
			attrVec2.y = 1.0f - mesh->mTextureCoords[0][i].y; 
			vertex.texCoords = attrVec2;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Traite toutes les faces du mesh (chaque indice)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Traite les matériaux/textures du mesh (sur base du modèle de Phong, on pourra implémenter PBR si on a le temps)
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// Récupère les diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// Récupère les specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// Récupère les normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// Récupère les height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	std::cout << "Mesh created with " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	// Traite toutes les textures du matériau
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		// Récupère le nom de l'image de la texture pour la charger 
		aiString imgName;
		mat->GetTexture(type, i, &imgName);
		std::string fileName = std::string(imgName.C_Str());
		fileName = directory + '/' + fileName;

		// Vérifie si la texture a déjà été chargée
		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (loadedTextures[j].path == fileName)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}
		// Si la texture n'a pas encore été chargée
		if (!skip)
		{
			Texture texture(fileName.c_str(), typeName.c_str(), i);
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}

	return textures;
}

void Model::calculateBoundingBox() {
	boundingBox = BoundingBox();

	for (const auto& mesh : meshes) {
		for (const auto& vertex : mesh.vertices) {
			boundingBox.update(vertex.position);
		}
	}
}
