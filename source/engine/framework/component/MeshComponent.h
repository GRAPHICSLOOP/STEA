#pragma once
#include <string>
#include "Component.h"
#include "../assets/ModelAsset.h"

class MeshComponent : public Component
{
public:
	MeshComponent(std::shared_ptr<class Object> obj);

	virtual void tick(float delta);

	void TempLoadMeshAsset(std::string name, std::string path);
	void setMeshAsset(std::shared_ptr<ModelAsset> asset);

private:
	std::shared_ptr<ModelAsset> mMeshAsset;
};




