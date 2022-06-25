#pragma once
#include <vector>
#include <memory>
#include "../object/Object.h"

// 有时间在优化对象管理

class Level
{
public:
	void initialize();
	void tick(float delta);

	std::shared_ptr<Object> createObject();
	void addObject(std::shared_ptr<Object> object);
	void removeObject(std::shared_ptr<Object> object);
	std::vector<std::shared_ptr<Object>> getObjects();

private:
	std::vector<std::shared_ptr<Object>> mObjects;
};



