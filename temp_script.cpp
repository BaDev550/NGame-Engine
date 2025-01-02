#ifndef PLAYERSCRIPT_H
#define PLAYERSCRIPT_H

#include "E:/CPP_Projects/Minecraft/Engine/Engine.h"

class PlayerScript : public Component {
public:
    CRigidBodyComponent* rb;

    void Start() override {
        rb = Owner->FindComponent<CRigidBodyComponent>();
        if (!rb) {
            std::cerr << "CRigidBodyComponent not found!" << std::endl;
        }
    }

    void Update(float deltaTime) override {
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
            if (rb) {
                rb->ApplyForce(glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime);
            }
        }

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
            if (rb) {
                rb->ApplyForce(glm::vec3(-1.0f, 0.0f, 0.0f) * deltaTime);
            }
        }
    }
};

#endif // PLAYERSCRIPT_H
