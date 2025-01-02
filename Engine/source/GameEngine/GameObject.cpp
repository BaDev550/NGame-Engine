#include "GameEngine/GameObject.h"

std::unordered_map<std::string, std::function<std::shared_ptr<Component>()>> GameObject::ComponentFactory;

bool GameObject::AddComponentByNameToObject(const std::string& componentName)
{
	auto it = ComponentFactory.find(componentName);
	if (it != ComponentFactory.end()) {
		auto component = it->second();
		Components.push_back(component);
		return true;
	}
	return false;

}

void GameObject::start()
{
    for (auto& component : Components) {
        component->Start();
    }
}

void GameObject::Update(float deltaTime)
{
    for (auto& component : Components) {
        component->Update(deltaTime);
        component->UpdateTransform(Position, Rotation, Size);
    }
}

void GameObject::Draw()
{
    for (auto& component : Components)
        component->Draw();
}