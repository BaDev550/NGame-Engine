#include "GameEngine/Components/BoxCollisionComponent/CBoxColliderComponent.h"

void CBoxCollisionComponent::RenderUI()
{
    ImGui::DragFloat3("Box Extent", &boxExtent[0], 0.1f, 0.0f, 100.0f);
}

bool CBoxCollisionComponent::CheckCollision(const CBoxCollisionComponent& other)
{
    return (Position.x - Size.x / 2 <= other.Position.x + other.Size.x / 2 &&
        Position.x + Size.x / 2 >= other.Position.x - other.Size.x / 2 &&
        Position.y - Size.y / 2 <= other.Position.y + other.Size.y / 2 &&
        Position.y + Size.y / 2 >= other.Position.y - other.Size.y / 2 &&
        Position.z - Size.z / 2 <= other.Position.z + other.Size.z / 2 &&
        Position.z + Size.z / 2 >= other.Position.z - other.Size.z / 2);
}

void CBoxCollisionComponent::Update(float deltaTime)
{

}

void CBoxCollisionComponent::UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size)
{
    Position = pos;
}

void CBoxCollisionComponent::Draw()
{
    ResourceManager::GetShader("SimpleShader").Use();
    ResourceManager::GetShader("SimpleShader").SetVector3f("color", Color);

    glPushMatrix();
    glColor3f(Color.r, Color.g, Color.b);
    glTranslatef(Position.x, Position.y, Position.z);
    glScalef(Size.x, Size.y, Size.z);

    glBegin(GL_LINES);

    glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, -0.5f);

    glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);

    glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glPopMatrix();

}
