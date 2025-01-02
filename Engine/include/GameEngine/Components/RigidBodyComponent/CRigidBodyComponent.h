#ifndef RIGIDBODY_COMPONENT_H
#define RIGIDBODY_COMPONENT_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <GameEngine/Components/Component.h>
#include <GameEngine/GameObject.h>

class CRigidBodyComponent : public Component
{
    glm::vec3 velocity;  
    glm::vec3 acceleration;
    glm::vec3 forces; 
    float mass;   
public:
    bool isStatic; 

    CRigidBodyComponent(float objectMass = 1.0f, bool staticObject = false)
        : velocity(0.0f), acceleration(0.0f), forces(0.0f), mass(objectMass), isStatic(staticObject) {}

    const char* GetName() const override {
        return "RigidBodyComponent";
    };

    void ApplyForce(const glm::vec3& force);
    void ClearForces();

    void Start() override {}
    void Update(float deltaTime) override;
    void Draw() override;
    void RenderUI() override;
    void UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size) override;

    glm::vec3 GetComponentPosition() override {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void SetPosition(glm::vec3 newPos) override {
    }

    std::string GetType() const override {
        return "Rigid Body";
    }

    void Serialize(nlohmann::json& json) const override {};
    void Deserialize(const nlohmann::json& json) override {};
};
#endif // !RIGIDBODY_COMPONENT_H

