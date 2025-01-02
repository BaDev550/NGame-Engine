#include <GameEngine/Components/RigidBodyComponent/CRigidBodyComponent.h>

void CRigidBodyComponent::ApplyForce(const glm::vec3& force)
{
    if (!isStatic)
        forces += force;
}

void CRigidBodyComponent::ClearForces()
{
    forces = glm::vec3(0.0f);
}

void CRigidBodyComponent::Update(float deltaTime)
{
    if (isStatic) {
        ClearForces();
        velocity = glm::vec3(0.0f);
        return;
    }

    acceleration = forces / mass;
    velocity += acceleration * deltaTime;

    if (Owner) {
        glm::vec3 position = Owner->GetPosition();
        Owner->Position = position + velocity * deltaTime;
    }

    ClearForces();
}

void CRigidBodyComponent::Draw()
{
}

void CRigidBodyComponent::RenderUI()
{
    ImGui::Text(std::to_string(velocity.y).c_str());
    ImGui::Checkbox("Is Static", &isStatic);
    ImGui::DragFloat("Mass", &mass, 0.5f);
}

void CRigidBodyComponent::UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size)
{
}


