#ifndef STATIC_MESH_COMPONENT_H
#define STATIC_MESH_COMPONENT_H

#include <glad/glad.h> 

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <RenderEngine/Mesh/Mesh.h>
#include <RenderEngine/Shader/Texture.h>
#include <GameEngine/Components/Component.h>
#include <ResourceManager.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using std::vector;
namespace fs = std::filesystem;

class CStaticMeshComponent : public Component
{
public:
    std::string currentPath = fs::current_path().string();
    std::string selectedFile;
    bool showFileBrowser = false;

    glm::vec3 Position;
    glm::vec3 Size;

    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    string Modelpath;
    bool gammaCorrection;

    const char* GetName() const override {
        return "StaticMeshComponent";
    }
    void RenderUI() override;
    void FileBrowser();

    CStaticMeshComponent(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f))
        : Position(position), Size(size) {}

    void Start() override {};
    void AddModel(string const& path, bool gamma = false) {
        gammaCorrection = gamma;
        Modelpath = path;
        loadModel(path);
    }
    void DrawMesh();
    void Draw() override;
    void UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size) override;

    glm::vec3 GetPosition() {
        return Position;
    }

    glm::vec3 getMinBoundingBox(glm::vec3 pos) const {
        glm::vec3 minBBox = glm::vec3(INFINITY);
        if (meshes.empty()) {
            return glm::vec3(0.0f);
        }

        for (const auto& mesh : meshes) {
            assert(&mesh != nullptr);
            glm::vec3 meshMin = mesh.getMinBoundingBox(pos);
            glm::vec3 meshMax = mesh.getMaxBoundingBox(pos);

            minBBox.x = std::min(minBBox.x, meshMin.x);
            minBBox.y = std::min(minBBox.y, meshMin.y);
            minBBox.z = std::min(minBBox.z, meshMin.z);
        }

        return minBBox;
    }

    glm::vec3 getMaxBoundingBox(glm::vec3 pos) const {
        glm::vec3 maxBBox = glm::vec3(-INFINITY);

        if (meshes.empty()) {
            return glm::vec3(0.0f);
        }

        for (const auto& mesh : meshes) {
            assert(&mesh != nullptr);
            glm::vec3 meshMin = mesh.getMinBoundingBox(pos);
            glm::vec3 meshMax = mesh.getMaxBoundingBox(pos);

            maxBBox.x = std::max(maxBBox.x, meshMax.x);
            maxBBox.y = std::max(maxBBox.y, meshMax.y);
            maxBBox.z = std::max(maxBBox.z, meshMax.z);
        }

        return maxBBox;
    }

    glm::vec3 GetComponentPosition() override {
        return Position;
    }

    void SetPosition(glm::vec3 newPos) override {
        Position = newPos;
    }

    std::string GetType() const override {
        return "Static Mesh";
    }

    void Serialize(nlohmann::json& json) const override {
        json["meshPath"] = Modelpath;
    };
    void Deserialize(const nlohmann::json& json) override {
        Modelpath = json["meshPath"];
        loadModel(Modelpath);
    };
private:
    void deleteTextures();
    void deleteMeshes();
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif