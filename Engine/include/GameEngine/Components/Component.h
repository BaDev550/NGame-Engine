#ifndef COMPONENT_BASE_H
#define COMPONENT_BASE_H
#pragma once

#include <imgui/imgui.h>
#include <filesystem>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <nlohmann/json.hpp>

class GameObject;

class Component {
public:
    GameObject* Owner;
    void SetOwner(GameObject* NewOwner) { Owner = NewOwner; }

    virtual glm::vec3 GetComponentPosition() = 0;
    virtual void SetPosition(glm::vec3 newPos) {};

    virtual ~Component() = default;
    virtual const char* GetName() const = 0;
    virtual void Start() {};
    virtual void Update(float deltaTime) {}
    virtual void Draw() {}
    virtual void RenderUI() = 0;
    virtual void UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size) {};

    virtual std::string GetType() const = 0;

    virtual void Serialize(nlohmann::json& json) const {};
    virtual void Deserialize(const nlohmann::json& json) {};
};

#endif // !COMPONENT_BASE_H