#include <GameEngine/Components/TransformComponent/CTransformComponent.h>

void CTransformComponent::RenderUI()
{
    //displayProperties();
}

glm::mat4 CTransformComponent::getModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void CTransformComponent::displayProperties()
{
    //ImGui::Text("Transform");
    //ImGui::DragFloat3("Position", &position[0], 0.1f);
    //ImGui::DragFloat3("Rotation", &rotation[0], 0.1f);
    //ImGui::DragFloat3("Scale", &scale[0], 0.1f);
}
