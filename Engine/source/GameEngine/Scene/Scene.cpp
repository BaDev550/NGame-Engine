#include <GameEngine/Scene/Scene.h>

void Scene::AddGameObject(std::string name) 
{ 
    GameObject* newObject = new GameObject(name);
    if (newObject) {
        GameObjects[name] = newObject;
    }
    else {
        std::cerr << "Failed to create a new object!" << std::endl;
    }
}

GameObject* Scene::GetGameObject(std::string name) { 
    return GameObjects[name];
}

GameObject* Scene::GetLastGameObject()
{
    std::string lastName = GameObjects.cbegin()->second->name;
    return GameObjects[lastName];
}

void Scene::start()
{
    for (auto& obj : GameObjects) {
        obj.second->start();
    }
}

void Scene::update(float dt)
{
    for (auto& obj : GameObjects) {
        //if (obj.second->GetComponent<CRigidBodyComponent>() != nullptr) {
        //    obj.second->GetComponent<CRigidBodyComponent>()->ApplyForce(glm::vec3(0.0f, -10.0f, 0.0f));
        //}
        obj.second->Update(dt);
    }
}

void Scene::CheckCollision() {
    for (auto itA = GameObjects.begin(); itA != GameObjects.end(); ++itA) {
        for (auto itB = std::next(itA); itB != GameObjects.end(); ++itB) {
            GameObject* objA = itA->second;
            GameObject* objB = itB->second;

            if (objA && objB) {
                auto colliderA = objA->GetComponent<CBoxCollisionComponent>();
                auto colliderB = objB->GetComponent<CBoxCollisionComponent>();

                if (colliderA && colliderB) {
                    bool collision = colliderA->CheckCollision(*colliderB);
                    colliderA->IsColliding = collision;
                    colliderB->IsColliding = collision;

                    if (collision) {
                        colliderA->Color = glm::vec3(1.0f, 0.0f, 0.0f);
                        colliderB->Color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    else {
                        colliderA->Color = glm::vec3(0.0f, 1.0f, 0.0f);
                        colliderB->Color = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                }
            }
        }
    }
}

void Scene::updateTransforms()
{
    for (auto& obj : GameObjects) {
        obj.second->Position = obj.second->Position;
        for (auto& comp : obj.second->Components) {
            comp->UpdateTransform(
                obj.second->Position,
                obj.second->Rotation,
                obj.second->Size
            );
        }
    }
}

void Scene::render(BDE3DE& renderEngine)
{
    renderEngine.render(GameObjects);
}

void Scene::Save(const std::string& filename)
{
    nlohmann::json jsonScene;

    for (const auto& pair : GameObjects) {
        const std::string& name = pair.first;
        GameObject* gameObject = pair.second;

        nlohmann::json jsonObject;
        jsonObject["name"] = name;
        jsonObject["position"] = { gameObject->Position.x, gameObject->Position.y, gameObject->Position.z };
        jsonObject["size"] = { gameObject->Size.x, gameObject->Size.y, gameObject->Size.z };
        jsonObject["rotation"] = { gameObject->Rotation.x, gameObject->Rotation.y, gameObject->Rotation.z };
        jsonObject["velocity"] = { gameObject->Velocity.x, gameObject->Velocity.y, gameObject->Velocity.z };
        jsonObject["isVisible"] = gameObject->IsVisible;

        for (const auto& component : gameObject->Components) {
            nlohmann::json jsonComponent;
            jsonComponent["type"] = component->GetType();
            jsonComponent["position"] = { component->GetComponentPosition().x, component->GetComponentPosition().y, component->GetComponentPosition().z};
            component->Serialize(jsonComponent);
            jsonObject["components"].push_back(jsonComponent);
        }

        jsonScene["GameObjects"].push_back(jsonObject);
    }

    std::ofstream file(filename);
    file << jsonScene.dump(4);
}

void Scene::Load(const std::string& filename)
{
    std::ifstream file(filename);
    nlohmann::json jsonScene;
    file >> jsonScene;

    GameObjects.clear();

    for (const auto& jsonObject : jsonScene["GameObjects"]) {
        std::string name = jsonObject["name"];
        auto* gameObject = new GameObject(name);

        gameObject->Position = glm::vec3(
            jsonObject["position"][0], jsonObject["position"][1], jsonObject["position"][2]);
        gameObject->Size = glm::vec3(
            jsonObject["size"][0], jsonObject["size"][1], jsonObject["size"][2]);
        gameObject->Rotation = glm::vec3(
            jsonObject["rotation"][0], jsonObject["rotation"][1], jsonObject["rotation"][2]);
        gameObject->Velocity = glm::vec3(
            jsonObject["velocity"][0], jsonObject["velocity"][1], jsonObject["velocity"][2]);
        gameObject->IsVisible = jsonObject["isVisible"];

        if (jsonObject.contains("components")) {
            for (const auto& jsonComponent : jsonObject["components"]) {
                std::string type = jsonComponent["type"];
                auto component = GameObject::ComponentFactory[type]();
                component->SetPosition(glm::vec3(jsonComponent["position"][0], jsonComponent["position"][1], jsonComponent["position"][2]));
                component->Deserialize(jsonComponent);  
                gameObject->Components.push_back(component);
                component->SetOwner(gameObject);
            }
        }

        GameObjects[name] = gameObject;
    }
}

