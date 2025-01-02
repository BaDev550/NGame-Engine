#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <GameEngine/Components/Component.h>

class CTransformComponent : public Component {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    const char* GetName() const override {
        return "TransformComponent";
    }

    void RenderUI() override;

    CTransformComponent()
        : position(0.0f), rotation(0.0f), scale(1.0f) {}

    glm::mat4 getModelMatrix() const;
    void displayProperties();
};

#endif // !TRANSFORM_COMPONENT_H