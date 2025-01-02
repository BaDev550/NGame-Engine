#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <GameEngine/Components/Component.h>
#include <GameEngine/Components/BoxCollisionComponent/CBoxColliderComponent.h>
#include <GameEngine/Components/StaticMeshComponent/CStaticMeshComponent.h>

#include <GameEngine/GameObject.h>
#include "Raycast/RayCast.h"

#include <map>
#include <string>

#include <RenderEngine/BDE3DRenderEngine.h>

#include <fstream>
#include <nlohmann/json.hpp>

class BDE3DE;

class Scene
{
public:
	Scene() {}

	std::map<std::string, GameObject*> GameObjects;

	void AddGameObject(std::string name);
	GameObject* GetGameObject(std::string name);
	GameObject* GetLastGameObject();

	void start();
	void update(float dt);
	void CheckCollision();
	void updateTransforms();
	void render(BDE3DE &renderEngine);

	void Save(const std::string& filename);
	void Load(const std::string& filename);
};
#endif // !SCENE_H

