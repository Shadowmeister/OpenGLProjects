#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path);
	void Draw(const Shader& shader);
private:
	/* Model data */
	std::vector<Mesh> meshes;
	std::string directory;
	/* Functions */
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		const Texture::TextureTypes textureType);

	std::vector<Texture> textures_loaded;
};

#endif