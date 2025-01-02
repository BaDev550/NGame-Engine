#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H
#pragma once

#include <memory>
#include <vector>

#include <GameEngine/Components/Component.h>
#include <GameEngine/GameObject.h>

class CBoxCollisionComponent : public Component {
public:
    glm::vec3 Position;
    glm::vec3 Size;
    glm::vec3 Color;
    bool IsColliding;

    glm::vec3 boxExtent;

    const char* GetName() const override {
        return "BoxCollisionComponent";
    }

    void RenderUI() override;

    CBoxCollisionComponent(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 color = glm::vec3(1.0f))
        : Position(position), Size(size), Color(color) {}

    bool CheckCollision(const CBoxCollisionComponent& other);

    void setSize(glm::vec3 size) { Size = size; }
    void Update(float deltaTime) override;

    void Start() override {};
    void UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size) override;
    void Draw() override;

    glm::vec3 GetComponentPosition() override{
        return Position;
    }

    void SetPosition(glm::vec3 newPos) override {
        Position = newPos;
    }

    std::string GetType() const override {
        return "Box Collision";
    }

    void Serialize(nlohmann::json& json) const override {};
    void Deserialize(const nlohmann::json& json) override {};
};

#endif // !BOX_COLLIDER_COMPONENT_H