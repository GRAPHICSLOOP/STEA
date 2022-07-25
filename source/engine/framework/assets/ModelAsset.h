#pragma once
#include <vector>
#include "function/render/renderResource/MeshResource.h"
#include "Texture2D.h"
#include "../../function/render/shader/Material.h"


struct MeshPart
{
public:
	std::shared_ptr<MeshResource> mMeshResource;
	std::shared_ptr<Texture2D> mTexture2D;
	Material* mMaterial;
};

class ModelAsset
{
public:
	ModelAsset(std::string name, std::string path);
	std::vector<struct ModelRenderResource> getMeshResource();
	void loadModel();

public:
	std::string mName;
	std::string mSourcePath;

private:
	void processNode(struct aiNode* node,const struct aiScene* scene);
	void processMesh(struct aiMesh* mesh, const struct aiScene* scene);
	std::shared_ptr<class Texture2D> loadMaterialTextures(struct aiMaterial* mat,enum aiTextureType type, std::string typeName);

private:
	std::vector<MeshPart> mMeshParts;
	std::string mDirectory;
};


