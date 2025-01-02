#include <GameEngine/Scene/Raycast/RayCast.h>

glm::vec3 RayCast::getRayFromMouse(const glm::vec2& mousePos, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos)
{
    glm::vec4 viewport(0, 0, windowWidth, windowHeight);
    glm::vec3 nearPoint = glm::unProject(glm::vec3(mousePos, 0.0f), view, projection, viewport);
    glm::vec3 farPoint = glm::unProject(glm::vec3(mousePos, 1.0f), view, projection, viewport);

    glm::vec3 rayDir = glm::normalize(farPoint - nearPoint);
    return rayDir;
}
