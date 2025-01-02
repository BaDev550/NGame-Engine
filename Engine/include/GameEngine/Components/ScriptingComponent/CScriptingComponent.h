#ifndef SCRIPTING_COMPONENT_H
#define SCRIPTING_COMPONENT_H

#include <GameEngine/Components/Component.h>
#include <GameEngine/Components/RigidBodyComponent/CRigidBodyComponent.h>
#include <glm/vec3.hpp>
#include <filesystem>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <functional>
#include <unordered_map>
#pragma once

class CScriptingComponent : public Component {
public:
    std::string ScriptPath;
    std::string currentPath = std::filesystem::current_path().string();
    std::string selectedFile;
    std::string ScriptName;

    std::unordered_map<std::string, std::function<void()>> methodMap;

    void Start() override {
        if (ScriptPath != "") {
            CompileAndExecuteCppScript(ScriptPath);
        }
    }

    glm::vec3 GetComponentPosition() override {
        return glm::vec3(0);
    }
    void SetPosition(glm::vec3 newPos) override {}
    const char* GetName() const override {
        if (ScriptPath != "") {
            return ScriptName.c_str();
        }
        return "Empty Script";
    }
    void Draw() override {}
    void RenderUI() override;
    void ShowFileBrowser();
    void UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size) override {}
    std::string GetType() const override {
        return "BDScript";
    }
    void Serialize(nlohmann::json& json) const override {}
    void Update(float deltaTime) {

    }
    void Deserialize(const nlohmann::json& json) override {}
    CScriptingComponent() {
        methodMap["ApplyForce"] = [this]() {
            CRigidBodyComponent* rb = Owner->FindComponent<CRigidBodyComponent>();
            if (rb) {
                rb->ApplyForce(glm::vec3(0, 10, 0));
            }
        };
    }

private:
    std::string TranslateCppScript(const std::string& cppContent);
    void CompileAndExecuteCppScript(const std::string& cppCode);
    void ParseAndRunScript(const std::string& scriptContent);
    void ExecuteFunction(const std::string& functionName);
};

#endif // SCRIPTING_COMPONENT_H
