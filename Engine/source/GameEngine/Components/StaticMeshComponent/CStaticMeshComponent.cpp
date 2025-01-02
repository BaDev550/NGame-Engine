#include "GameEngine/Components/StaticMeshComponent/CStaticMeshComponent.h"

void CStaticMeshComponent::DrawMesh()
{
    ResourceManager::GetShader("DefaultShader").SetFloat("material.shininess", 32.0f);
    for (unsigned int i = 0; i < meshes.size(); i++) {

        for (unsigned int i = 0; i < meshes[i].textures.size(); i++)
        {
            ResourceManager::GetTexture(meshes[i].textures[i].type).Bind();
            ResourceManager::GetShader("DefaultShader").SetInteger(("material." + meshes[i].textures[i].type).c_str(), i);
        }
		meshes[i].Draw();
    }
}

void CStaticMeshComponent::deleteTextures()
{
    for (auto& texture : textures_loaded)
    {
        glDeleteTextures(1, &texture.id);
    }
    textures_loaded.clear();
}

void CStaticMeshComponent::deleteMeshes()
{
    for (auto& mesh : meshes)
    {
        
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }

    meshes.clear();
}

void CStaticMeshComponent::Draw()
{
    DrawMesh();
}

void CStaticMeshComponent::UpdateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 size)
{
    Position = pos;
    Size = size;
}

void CStaticMeshComponent::loadModel(string const& path)
{
    deleteTextures();
    deleteMeshes();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    std::cout << "Model loaded" << std::endl;
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    std::cout << "Model Path: " << path << std::endl;
}

void CStaticMeshComponent::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh CStaticMeshComponent::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures);
}

vector<Texture> CStaticMeshComponent::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = ResourceManager::LoadTextureDir(str.C_Str(), this->directory).ID;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

std::string ConvertPathToUnixStyle(const std::string& path) {
    std::string convertedPath = path;
    std::replace(convertedPath.begin(), convertedPath.end(), '\\', '/');
    return convertedPath;
}

void CStaticMeshComponent::FileBrowser() {
    if (!showFileBrowser) return;

    ImGui::Begin("File Browser");

    ImGui::Text("Current Path: %s", currentPath.c_str());
    ImGui::Separator();

    if (ImGui::Button("Up")) {
        if (fs::path(currentPath).has_parent_path()) {
            currentPath = fs::path(currentPath).parent_path().string();
        }
    }

    for (const auto& entry : fs::directory_iterator(currentPath)) {
        const std::string name = entry.path().filename().string();
        const bool isDirectory = entry.is_directory();

        if (isDirectory) {
            if (ImGui::Selectable((name + "/").c_str())) {
                currentPath = entry.path().string();
            }
        }
        else {
            if (ImGui::Selectable(name.c_str())) {
                selectedFile = entry.path().string();
            }
        }
    }
    
    if (!selectedFile.empty()) {
        ImGui::Separator();
        ImGui::Text("Selected File: %s", selectedFile.c_str());

        if (ImGui::Button("Load Model")) {
            AddModel(ConvertPathToUnixStyle(selectedFile));
            selectedFile.clear();
        }
    }
    ImGui::End();
}

void CStaticMeshComponent::RenderUI(){
    if (ImGui::Button("Load Model")) {
        showFileBrowser = showFileBrowser ? false : true;
    }

    FileBrowser();
}
