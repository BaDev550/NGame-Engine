#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Components/Component.h"
#include <ResourceManager.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

class CRigidBodyComponent;
class CBoxCollisionComponent;
class CStaticMeshComponent;

class GameObject
{
protected:
	glm::vec3 Color;
	bool Destroyed;
	bool HasCollision;
public:
	glm::vec3 Position, Size, Rotation, Velocity;
	std::string name;
	bool IsVisible;

	GameObject() 
		: Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), IsVisible(true), Destroyed(false) {
	};

	GameObject(std::string ObjectsName) 
		: name(ObjectsName), Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), IsVisible(true), Destroyed(false) {
	};

	GameObject(glm::vec3 pos, glm::vec3 size, bool hasCollision, glm::vec3 color, glm::vec3 velocity)
		: Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), IsVisible(true), Destroyed(false) {
	};

	std::vector<std::shared_ptr<Component>> Components;

	template <typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&&... args);
	bool AddComponentByNameToObject(const std::string& componentName);
	template <typename T>
	std::shared_ptr<T> GetComponent();
	template <typename T>
	T* FindComponent();

	void RenderComponentsUI() {
		for (auto& comp : Components) {
			comp->RenderUI();
		}
	}

	void SetPosition(glm::vec3 pos) {
		Position = pos;
	}

	void start();
	void Update(float deltaTime);
	virtual void Draw();

	__forceinline glm::vec3 GetPosition() { return Position; }
	static void RegisterComponent(const std::string& name, std::function<std::shared_ptr<Component>()> creator) {
		ComponentFactory[name] = creator;
	}
	static std::unordered_map<std::string, std::function<std::shared_ptr<Component>()>> ComponentFactory;
};

#endif // !GAMEOBJECT_CLASS_HEADER

template<typename T, typename ...Args>
inline std::shared_ptr<T> GameObject::AddComponent(Args && ...args)
{
	auto component = std::make_shared<T>(std::forward<Args>(args)...);
	Components.push_back(component);
	return component;
}

template<typename T>
inline std::shared_ptr<T> GameObject::GetComponent()
{
	for (auto& component : Components) {
		auto castedComponent = std::dynamic_pointer_cast<T>(component);
		if (castedComponent)
			return castedComponent;
	}
	return nullptr;
}

template<typename T>
inline T* GameObject::FindComponent()
{
	for (auto& component : Components) {
		T* derived = dynamic_cast<T*>(component.get());
		if (derived) {
			return derived;
		}
	}
	return nullptr;
}
