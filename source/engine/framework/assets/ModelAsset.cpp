#include "ModelAsset.h"
#include "core/base/macro.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "core/base/macro.h"

ModelAsset::ModelAsset(std::string name, std::string path)
{
	mName = name;
	mSourcePath = path;
}

std::vector<ModelRenderResource> ModelAsset::getMeshResource()
{
	std::vector<ModelRenderResource> modelRenderResource;
	for (auto& mesh : mMeshParts)
	{
		ModelRenderResource resource;
		resource.mMeshResource = mesh.mMeshResource.get();
		resource.mMaterial = mesh.mMaterial;
		modelRenderResource.push_back(resource);
	}
	return modelRenderResource;
}

void ModelAsset::loadModel()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(mSourcePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		STEALOG_ERROR("Import failed");
		return;
	}

	mDirectory = mSourcePath.substr(0, mSourcePath.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void ModelAsset::processNode(aiNode* node, const aiScene* scene)
{
	// 先Node处理
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		auto* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	// 遍历子Node处理Mesh
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void ModelAsset::processMesh(aiMesh* mesh, const aiScene* scene)
{
	MeshPart meshPart;
	std::vector<VertexBufferData> vertices;
	std::vector<uint32_t> indices;
	vertices.resize(mesh->mNumVertices);

	// vertices
	if (mesh->mTextureCoords[0])
	{
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			VertexBufferData vertex;
			vertex.mPosition.x = mesh->mVertices[i].x;
			vertex.mPosition.y = mesh->mVertices[i].y;
			vertex.mPosition.z = mesh->mVertices[i].z;
			vertex.mTexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.mTexCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.mNormal.x = mesh->mNormals[i].x;
			vertex.mNormal.y = mesh->mNormals[i].y;
			vertex.mNormal.z = mesh->mNormals[i].z;
			vertices[i] = vertex;
		}
	}
	else
	{
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			VertexBufferData vertex;
			vertex.mPosition.x = mesh->mVertices[i].x;
			vertex.mPosition.y = mesh->mVertices[i].y;
			vertex.mPosition.z = mesh->mVertices[i].z;
			vertex.mTexCoord = glm::vec2(0.f);
			vertex.mNormal.x = mesh->mNormals[i].x;
			vertex.mNormal.y = mesh->mNormals[i].y;
			vertex.mNormal.z = mesh->mNormals[i].z;
			vertices[i] = vertex;
		}
	}

	// indices
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		const char* name = material->GetName().C_Str();
		meshPart.mTexture2D = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		std::vector<MaterialAttribute> materialAttribute =
		{
			MaterialAttribute("texSampler",meshPart.mTexture2D.get()->getId())
		};	
		meshPart.mMaterial = gRuntimeGlobalContext.getRenderResource()->createMaterial(name, gRuntimeGlobalContext.getRenderResource()->getShader("obj"), materialAttribute);
	}
	else
	{
		STEALOG_WARN("该mesh没有对应的贴图: {}", mesh->mName.C_Str());
	}

	meshPart.mMeshResource = std::make_shared<MeshResource>(vertices, indices);

	mMeshParts.push_back(meshPart);
}

std::shared_ptr<Texture2D> ModelAsset::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	aiString str;
	mat->GetTexture(type, 0, &str);

	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
	texture->initialize(mDirectory + '/' + str.C_Str());
	return texture;
}
